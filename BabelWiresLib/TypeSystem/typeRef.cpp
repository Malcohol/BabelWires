/**
 * A TypeRef identifies a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Hash/hash.hpp>

#include <bitset>

namespace {
    // I would rather use '<' and '>', but they get escaped in XML, which makes project files ugly.
    constexpr char openChar = '[';
    constexpr char closeChar = ']';
    constexpr char defaultStateString[] = "[]";
    constexpr char parseChars[] = "[],";
} // namespace

babelwires::TypeRef::TypeRef() = default;

babelwires::TypeRef::TypeRef(PrimitiveTypeId typeId)
    : m_storage(typeId) {}

babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeConstructorArgumentsOld arguments)
    : m_storage(ConstructedTypeData{typeConstructorId, std::move(arguments)}) {
    assert((arguments.m_typeArguments.size() <= TypeConstructorArgumentsOld::s_maxNumArguments) &&
           "Too many arguments for TypeRef");
}

const babelwires::Type* babelwires::TypeRef::tryResolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        const babelwires::Type* operator()(std::monostate) { return nullptr; }
        const babelwires::Type* operator()(PrimitiveTypeId typeId) { return m_typeSystem.tryGetPrimitiveType(typeId); }
        const babelwires::Type* operator()(const ConstructedTypeData& higherOrderData) {
            const TypeConstructorId typeConstructorId = std::get<0>(higherOrderData);
            const TypeConstructorArgumentsOld& args = std::get<1>(higherOrderData);
            if (args.m_typeArguments.size() == 1) {
                if (const TypeConstructor<1>* const typeConstructor =
                        m_typeSystem.tryGetTypeConstructor<1>(typeConstructorId)) {
                    TypeConstructorArguments<1> newArgs;
                    newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                    return typeConstructor->getOrConstructType(m_typeSystem, newArgs);
                }
            } else if (args.m_typeArguments.size() == 2) {
                if (const TypeConstructor<2>* const typeConstructor =
                        m_typeSystem.tryGetTypeConstructor<2>(typeConstructorId)) {
                    TypeConstructorArguments<2> newArgs;
                    newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                    newArgs.m_typeArguments[1] = args.m_typeArguments[1];
                    return typeConstructor->getOrConstructType(m_typeSystem, newArgs);
                }
            }
            return nullptr;
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{typeSystem};
    return std::visit(visitorMethods, m_storage);
}

const babelwires::Type& babelwires::TypeRef::resolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        const babelwires::Type& operator()(std::monostate) {
            throw TypeSystemException() << "A null type cannot be resolved.";
        }
        const babelwires::Type& operator()(PrimitiveTypeId typeId) { return m_typeSystem.getPrimitiveType(typeId); }
        const babelwires::Type& operator()(const ConstructedTypeData& higherOrderData) {
            const TypeConstructorId typeConstructorId = std::get<0>(higherOrderData);
            const TypeConstructorArgumentsOld& args = std::get<1>(higherOrderData);
            const Type* type = nullptr;
            if (args.m_typeArguments.size() == 1) {
                if (const TypeConstructor<1>* const typeConstructor =
                        m_typeSystem.tryGetTypeConstructor<1>(typeConstructorId)) {
                    TypeConstructorArguments<1> newArgs;
                    newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                    type = typeConstructor->getOrConstructType(m_typeSystem, newArgs);
                }
            } else if (args.m_typeArguments.size() == 2) {
                if (const TypeConstructor<2>* const typeConstructor =
                        m_typeSystem.tryGetTypeConstructor<2>(typeConstructorId)) {
                    TypeConstructorArguments<2> newArgs;
                    newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                    newArgs.m_typeArguments[1] = args.m_typeArguments[1];
                    type = typeConstructor->getOrConstructType(m_typeSystem, newArgs);
                }
            }
            assert(type);
            return *type;
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{typeSystem};
    return std::visit(visitorMethods, m_storage);
}

std::string babelwires::TypeRef::serializeToString() const {
    struct VisitorMethods {
        std::string operator()(std::monostate) { return defaultStateString; }
        std::string operator()(PrimitiveTypeId typeId) { return typeId.serializeToString(); }
        std::string operator()(const ConstructedTypeData& higherOrderData) {
            std::ostringstream os;
            os << std::get<0>(higherOrderData).serializeToString();
            const auto& arguments = std::get<1>(higherOrderData).m_typeArguments;
            if (arguments.empty()) {
                // Correctly formed TypeRef should not hit this case.
                // It can be hit when falling-back to this representation
                // if toString formatting fails.
                os << defaultStateString;
            } else {
                char sep = openChar;
                for (const auto& arg : arguments) {
                    os << sep << arg.serializeToString();
                    sep = ',';
                }
                os << closeChar;
            }
            return os.str();
        }
    } visitorMethods;
    return std::visit(visitorMethods, m_storage);
}

namespace {
    /// A very simple string formatting algorithm sufficient for this job.
    /// Hopefully could be replaced by std::format when that's properly supported.
    std::string format(std::string_view format, const std::vector<std::string>& arguments) {
        if (format.size() < 2) {
            return {};
        }
        if (arguments.size() > babelwires::TypeConstructorArgumentsOld::s_maxNumArguments) {
            return {};
        }
        std::bitset<babelwires::TypeConstructorArgumentsOld::s_maxNumArguments> argumentsNotYetSeen(
            (1 << arguments.size()) - 1);
        std::ostringstream oss;
        constexpr char open = '{';
        constexpr char close = '}';
        enum { normal, justReadOpen, justReadIndex, justReadClose } state = normal;
        for (const char currentChar : format) {
            switch (state) {
                case normal: {
                    if (currentChar == open) {
                        state = justReadOpen;
                    } else if (currentChar == close) {
                        state = justReadClose;
                    } else {
                        oss << currentChar;
                    }
                    break;
                }
                case justReadOpen: {
                    static_assert(babelwires::TypeConstructorArgumentsOld::s_maxNumArguments == 10);
                    if ((currentChar >= '0') && (currentChar <= '9')) {
                        const std::size_t indexCharAsIndex = currentChar - '0';
                        if (indexCharAsIndex >= arguments.size()) {
                            return {};
                        }
                        oss << arguments[indexCharAsIndex];
                        argumentsNotYetSeen.reset(indexCharAsIndex);
                        state = justReadIndex;
                    } else if (currentChar == open) {
                        oss << open;
                        state = normal;
                    } else {
                        return {};
                    }
                    break;
                }
                case justReadIndex: {
                    if (currentChar == close) {
                        state = normal;
                    } else {
                        return {};
                    }
                    break;
                }
                case justReadClose: {
                    if (currentChar == close) {
                        oss << close;
                        state = normal;
                    } else {
                        return {};
                    }
                    break;
                }
            }
        }

        if (argumentsNotYetSeen.none()) {
            return oss.str();
        } else {
            return {};
        }
    }
} // namespace

std::string babelwires::TypeRef::toStringHelper(babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const {
    struct VisitorMethods {
        std::string operator()(std::monostate) { return defaultStateString; }
        std::string operator()(PrimitiveTypeId typeId) { return m_identifierRegistry->getName(typeId); }
        std::string operator()(const ConstructedTypeData& higherOrderData) {
            std::string formatString = m_identifierRegistry->getName(std::get<0>(higherOrderData));
            std::vector<std::string> argumentsStr;
            const auto& arguments = std::get<1>(higherOrderData).m_typeArguments;
            std::for_each(arguments.begin(), arguments.end(), [this, &argumentsStr](const TypeRef& typeRef) {
                argumentsStr.emplace_back(typeRef.toStringHelper(m_identifierRegistry));
            });
            return format(formatString, argumentsStr);
        }
        babelwires::IdentifierRegistry::ReadAccess& m_identifierRegistry;
    } visitorMethods{identifierRegistry};
    const std::string str = std::visit(visitorMethods, m_storage);
    if (!str.empty()) {
        return str;
    } else {
        return "MalformedTypeRef{" + serializeToString() + "}";
    }
}

std::string babelwires::TypeRef::toString() const {
    auto regScope = IdentifierRegistry::read();
    return toStringHelper(regScope);
}

std::tuple<babelwires::TypeRef, std::string_view::size_type> babelwires::TypeRef::parseHelper(std::string_view str) {
    std::string_view::size_type next = 0;
    auto IdEnd = str.find_first_of(parseChars, next);
    if (IdEnd == -1) {
        return {TypeRef{LongIdentifier::deserializeFromString(str)}, str.size()};
    }
    if ((IdEnd + 1 < str.size()) && (IdEnd == next) && (str[IdEnd] == openChar) && (str[IdEnd + 1] == closeChar)) {
        return {TypeRef(), IdEnd + 2};
    }
    const MediumId startId = MediumId::deserializeFromString(str.substr(next, IdEnd));
    next = IdEnd;
    if (str[next] != openChar) {
        return {startId, next};
    }
    ++next;
    TypeConstructorArgumentsOld arguments;
    if (next == str.size()) {
        throw ParseException() << "Unterminated TypeRef";
    }
    while (1) {
        auto tuple = parseHelper(str.substr(next));
        arguments.m_typeArguments.emplace_back(std::move(std::get<0>(tuple)));
        if (arguments.m_typeArguments.size() > TypeConstructorArgumentsOld::s_maxNumArguments) {
            throw ParseException() << "TypeRef too many arguments (maximum allowed is "
                                   << TypeConstructorArgumentsOld::s_maxNumArguments << ")";
        }
        assert((std::get<1>(tuple) > 0) && "Did not advance while parsing");
        next += std::get<1>(tuple);
        if (next == str.size()) {
            throw ParseException() << "Unterminated TypeRef";
        }
        if (str[next] == ',') {
            ++next;
        } else if (str[next] == closeChar) {
            ++next;
            break;
        } else {
            throw ParseException() << "Trailing characters in inner TypeRef";
        }
    }
    return {TypeRef{startId, std::move(arguments)}, next};
}

babelwires::TypeRef babelwires::TypeRef::deserializeFromString(std::string_view str) {
    auto parseResult = parseHelper(str);
    if (std::get<1>(parseResult) != str.size()) {
        throw ParseException() << "Trailing characters in TypeRef";
    }
    return std::get<0>(parseResult);
}

void babelwires::TypeRef::visitIdentifiers(IdentifierVisitor& visitor) {
    // Note: The visitor needs to access the actual stored data, so be careful to avoid copies.
    struct VisitorMethods {
        void operator()(std::monostate) {}
        void operator()(PrimitiveTypeId& typeId) { m_visitor(typeId); }
        void operator()(ConstructedTypeData& higherOrderData) {
            m_visitor(std::get<0>(higherOrderData));
            auto& arguments = std::get<1>(higherOrderData).m_typeArguments;
            std::for_each(arguments.begin(), arguments.end(),
                          [this](TypeRef& arg) { arg.visitIdentifiers(m_visitor); });
        }
        IdentifierVisitor& m_visitor;
    } visitorMethods{visitor};
    return std::visit(visitorMethods, m_storage);
}

void babelwires::TypeRef::visitFilePaths(FilePathVisitor& visitor) {}

std::size_t babelwires::TypeRef::getHash() const {
    std::size_t hash = 0x123456789;
    // I wonder if the construction of std::hash objects creates pointless overhead here?
    struct VisitorMethods {
        void operator()(std::monostate) { hash::mixInto(m_currentHash, 0x11122233); }
        void operator()(const PrimitiveTypeId& typeId) { hash::mixInto(m_currentHash, typeId); }
        void operator()(const ConstructedTypeData& higherOrderData) {
            hash::mixInto(m_currentHash, std::get<0>(higherOrderData), std::get<1>(higherOrderData));
        }
        std::size_t& m_currentHash;
    } visitorMethods{hash};
    std::visit(visitorMethods, m_storage);
    return hash;
}

babelwires::SubtypeOrder babelwires::TypeRef::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                   const TypeRef& typeRefA, const TypeRef& typeRefB) {
    return std::visit(
        overloaded{
            [](std::monostate, std::monostate) { return SubtypeOrder::IsEquivalent; },
            [&typeSystem](const PrimitiveTypeId& typeIdA, const PrimitiveTypeId& typeIdB) {
                return typeSystem.compareSubtypePrimitives(typeIdA, typeIdB);
            },
            [&typeSystem](const ConstructedTypeData& higherOrderDataA, const PrimitiveTypeId& typeIdB) {
                const TypeConstructorId typeConstructorId = std::get<0>(higherOrderDataA);
                const TypeConstructorArgumentsOld& args = std::get<1>(higherOrderDataA);
                if (args.m_typeArguments.size() == 1) {
                    if (const TypeConstructor<1>* const typeConstructor =
                            typeSystem.tryGetTypeConstructor<1>(typeConstructorId)) {
                        TypeConstructorArguments<1> newArgs;
                        newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                        return typeConstructor->compareSubtypeHelper(typeSystem, newArgs, typeIdB);
                    }
                } else if (args.m_typeArguments.size() == 2) {
                    if (const TypeConstructor<2>* const typeConstructor =
                            typeSystem.tryGetTypeConstructor<2>(typeConstructorId)) {
                        TypeConstructorArguments<2> newArgs;
                        newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                        newArgs.m_typeArguments[1] = args.m_typeArguments[1];
                        return typeConstructor->compareSubtypeHelper(typeSystem, newArgs, typeIdB);
                    }
                }
                return SubtypeOrder::IsUnrelated;
            },
            [&typeSystem](const PrimitiveTypeId& typeIdA, const ConstructedTypeData& higherOrderDataB) {
                const TypeConstructorId typeConstructorIdB = std::get<0>(higherOrderDataB);
                const TypeConstructorArgumentsOld& args = std::get<1>(higherOrderDataB);
                if (args.m_typeArguments.size() == 1) {
                    if (const TypeConstructor<1>* const typeConstructor =
                            typeSystem.tryGetTypeConstructor<1>(typeConstructorIdB)) {
                        TypeConstructorArguments<1> newArgs;
                        newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                        return reverseSubtypeOrder(typeConstructor->compareSubtypeHelper(typeSystem, newArgs, typeIdA));
                    }
                } else if (args.m_typeArguments.size() == 2) {
                    if (const TypeConstructor<2>* const typeConstructor =
                            typeSystem.tryGetTypeConstructor<2>(typeConstructorIdB)) {
                        TypeConstructorArguments<2> newArgs;
                        newArgs.m_typeArguments[0] = args.m_typeArguments[0];
                        newArgs.m_typeArguments[1] = args.m_typeArguments[1];
                        return reverseSubtypeOrder(typeConstructor->compareSubtypeHelper(typeSystem, newArgs, typeIdA));
                    }
                }
                return SubtypeOrder::IsUnrelated;
            },
            [&typeSystem, &typeRefA, &typeRefB](const ConstructedTypeData& higherOrderDataA,
                                                const ConstructedTypeData& higherOrderDataB) {
                const TypeConstructorId typeConstructorIdA = std::get<0>(higherOrderDataA);
                const TypeConstructorId typeConstructorIdB = std::get<0>(higherOrderDataB);
                const TypeConstructorArgumentsOld& argsA = std::get<1>(higherOrderDataA);
                const TypeConstructorArgumentsOld& argsB = std::get<1>(higherOrderDataB);
                if (typeConstructorIdA == typeConstructorIdB) {
                    if (argsA.m_typeArguments.size() == argsB.m_typeArguments.size()) {
                        if (argsA.m_typeArguments.size() == 1) {
                            if (const TypeConstructor<1>* const typeConstructorA =
                                    typeSystem.tryGetTypeConstructor<1>(typeConstructorIdA)) {
                                TypeConstructorArguments<1> newArgsA;
                                newArgsA.m_typeArguments[0] = argsA.m_typeArguments[0];
                                TypeConstructorArguments<1> newArgsB;
                                newArgsB.m_typeArguments[0] = argsB.m_typeArguments[0];
                                return typeConstructorA->compareSubtypeHelper(typeSystem, newArgsA, newArgsB);
                            }
                        } else if (argsA.m_typeArguments.size() == 2) {
                            if (const TypeConstructor<2>* const typeConstructorA =
                                    typeSystem.tryGetTypeConstructor<2>(typeConstructorIdA)) {
                                TypeConstructorArguments<2> newArgsA;
                                newArgsA.m_typeArguments[0] = argsA.m_typeArguments[0];
                                newArgsA.m_typeArguments[1] = argsA.m_typeArguments[1];
                                TypeConstructorArguments<2> newArgsB;
                                newArgsB.m_typeArguments[0] = argsB.m_typeArguments[0];
                                newArgsB.m_typeArguments[1] = argsB.m_typeArguments[1];
                                return typeConstructorA->compareSubtypeHelper(typeSystem, newArgsA, newArgsB);
                            }
                        }
                    }
                }
                if (argsA.m_typeArguments.size() == 1) {
                    if (const TypeConstructor<1>* const typeConstructorA =
                            typeSystem.tryGetTypeConstructor<1>(typeConstructorIdA)) {
                        TypeConstructorArguments<1> newArgsA;
                        newArgsA.m_typeArguments[0] = argsA.m_typeArguments[0];
                        auto comparisonUsingA = typeConstructorA->compareSubtypeHelper(typeSystem, newArgsA, typeRefB);
                        if (comparisonUsingA != SubtypeOrder::IsUnrelated) {
                            return comparisonUsingA;
                        }
                        if (argsB.m_typeArguments.size() == 1) {
                            TypeConstructorArguments<1> newArgsB;
                            newArgsB.m_typeArguments[0] = argsB.m_typeArguments[0];
                            if (const TypeConstructor<1>* const typeConstructorB =
                                    typeSystem.tryGetTypeConstructor<1>(typeConstructorIdB)) {
                                return reverseSubtypeOrder(
                                    typeConstructorB->compareSubtypeHelper(typeSystem, newArgsB, typeRefA));
                            }
                        } else if (argsB.m_typeArguments.size() == 2) {
                            TypeConstructorArguments<2> newArgsB;
                            newArgsB.m_typeArguments[0] = argsB.m_typeArguments[0];
                            newArgsB.m_typeArguments[1] = argsB.m_typeArguments[1];
                            if (const TypeConstructor<2>* const typeConstructorB =
                                    typeSystem.tryGetTypeConstructor<2>(typeConstructorIdB)) {
                                return reverseSubtypeOrder(
                                    typeConstructorB->compareSubtypeHelper(typeSystem, newArgsB, typeRefA));
                            }
                        }
                    }
                } else if (argsA.m_typeArguments.size() == 2) {
                    if (const TypeConstructor<2>* const typeConstructorA =
                            typeSystem.tryGetTypeConstructor<2>(typeConstructorIdA)) {
                        TypeConstructorArguments<2> newArgsA;
                        newArgsA.m_typeArguments[0] = argsA.m_typeArguments[0];
                        newArgsA.m_typeArguments[1] = argsA.m_typeArguments[1];
                        auto comparisonUsingA = typeConstructorA->compareSubtypeHelper(typeSystem, newArgsA, typeRefB);
                        if (comparisonUsingA != SubtypeOrder::IsUnrelated) {
                            return comparisonUsingA;
                        }
                        if (argsB.m_typeArguments.size() == 1) {
                            TypeConstructorArguments<1> newArgsB;
                            newArgsB.m_typeArguments[0] = argsB.m_typeArguments[0];
                            if (const TypeConstructor<1>* const typeConstructorB =
                                    typeSystem.tryGetTypeConstructor<1>(typeConstructorIdB)) {
                                return reverseSubtypeOrder(
                                    typeConstructorB->compareSubtypeHelper(typeSystem, newArgsB, typeRefA));
                            }
                        } else if (argsB.m_typeArguments.size() == 2) {
                            TypeConstructorArguments<2> newArgsB;
                            newArgsB.m_typeArguments[0] = argsB.m_typeArguments[0];
                            newArgsB.m_typeArguments[1] = argsB.m_typeArguments[1];
                            if (const TypeConstructor<2>* const typeConstructorB =
                                    typeSystem.tryGetTypeConstructor<2>(typeConstructorIdB)) {
                                return reverseSubtypeOrder(
                                    typeConstructorB->compareSubtypeHelper(typeSystem, newArgsB, typeRefA));
                            }
                        }
                    }
                }
                return SubtypeOrder::IsUnrelated;
            },
            [](auto&&, auto&&) { return SubtypeOrder::IsUnrelated; }},
        typeRefA.m_storage, typeRefB.m_storage);
}
