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
babelwires::TypeRef::~TypeRef() = default;

babelwires::TypeRef::TypeRef(PrimitiveTypeId typeId)
    : m_storage(typeId) {}

babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeRef argument)
    : TypeRef(typeConstructorId, TypeConstructorArguments<1>{std::move(argument)}) {}

babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeRef argument0, TypeRef argument1)
    : TypeRef(typeConstructorId, TypeConstructorArguments<2>{std::move(argument0), std::move(argument1)}) {}

struct babelwires::TypeRef::TryResolveVisitor {
    const babelwires::Type* operator()(std::monostate) { return nullptr; }

    const babelwires::Type* operator()(PrimitiveTypeId typeId) { return m_typeSystem.tryGetPrimitiveType(typeId); }

    template <unsigned int N> const babelwires::Type* operator()(const ConstructedStorage<N>& higherOrderData) {
        const TypeConstructorId typeConstructorId = higherOrderData->m_typeConstructorId;
        const TypeConstructorArguments<N>& args = higherOrderData->m_arguments;
        if (const TypeConstructor<N>* const typeConstructor =
                m_typeSystem.tryGetTypeConstructor<N>(typeConstructorId)) {
            return typeConstructor->getOrConstructType(m_typeSystem, args);
        }
        return nullptr;
    }
    const TypeSystem& m_typeSystem;
};

const babelwires::Type* babelwires::TypeRef::tryResolve(const TypeSystem& typeSystem) const {
    return std::visit(TryResolveVisitor{typeSystem}, m_storage);
}

struct babelwires::TypeRef::ResolveVisitor {
    const babelwires::Type& operator()(std::monostate) {
        throw TypeSystemException() << "A null type cannot be resolved.";
    }
    const babelwires::Type& operator()(PrimitiveTypeId typeId) { return m_typeSystem.getPrimitiveType(typeId); }

    template <unsigned int N> const babelwires::Type& operator()(const ConstructedStorage<N>& higherOrderData) {
        const TypeConstructorId typeConstructorId = higherOrderData->m_typeConstructorId;
        const TypeConstructorArguments<N>& args = higherOrderData->m_arguments;
        const TypeConstructor<N>& typeConstructor = m_typeSystem.getTypeConstructor<N>(typeConstructorId);
        const Type* const type = typeConstructor.getOrConstructType(m_typeSystem, args);
        assert(type);
        return *type;
    }
    const TypeSystem& m_typeSystem;
};

const babelwires::Type& babelwires::TypeRef::resolve(const TypeSystem& typeSystem) const {
    return std::visit(ResolveVisitor{typeSystem}, m_storage);
}

struct babelwires::TypeRef::SerializeVisitor {
    std::string operator()(std::monostate) { return defaultStateString; }
    std::string operator()(PrimitiveTypeId typeId) { return typeId.serializeToString(); }

    template <unsigned int N> std::string operator()(const ConstructedStorage<N>& higherOrderData) {
        std::ostringstream os;
        os << higherOrderData->m_typeConstructorId.serializeToString();
        const auto& arguments = higherOrderData->m_arguments;
        char sep = openChar;
        for (const auto& arg : arguments.m_typeArguments) {
            os << sep << arg.serializeToString();
            sep = ',';
        }
        os << closeChar;
        return os.str();
    }
};

std::string babelwires::TypeRef::serializeToString() const {
    return std::visit(SerializeVisitor(), m_storage);
}

namespace {
    /// A very simple string formatting algorithm sufficient for this job.
    /// Hopefully could be replaced by std::format when that's properly supported.
    std::string format(std::string_view format, const std::vector<std::string>& arguments) {
        if (format.size() < 2) {
            return {};
        }
        if (arguments.size() > babelwires::c_maxNumTypeConstructorArguments) {
            return {};
        }
        std::bitset<babelwires::c_maxNumTypeConstructorArguments> argumentsNotYetSeen((1 << arguments.size()) - 1);
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
                    static_assert(babelwires::c_maxNumTypeConstructorArguments <= 10);
                    if ((currentChar >= '0') && (currentChar < '0' + babelwires::c_maxNumTypeConstructorArguments)) {
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

struct babelwires::TypeRef::ToStringVisitor {
    std::string operator()(std::monostate) { return defaultStateString; }
    std::string operator()(PrimitiveTypeId typeId) { return m_identifierRegistry->getName(typeId); }
    template <unsigned int N> std::string operator()(const ConstructedStorage<N>& higherOrderData) {
        std::string formatString = m_identifierRegistry->getName(higherOrderData->m_typeConstructorId);
        std::vector<std::string> argumentsStr;
        const auto& arguments = higherOrderData->m_arguments.m_typeArguments;
        std::for_each(arguments.begin(), arguments.end(), [this, &argumentsStr](const TypeRef& typeRef) {
            argumentsStr.emplace_back(typeRef.toStringHelper(m_identifierRegistry));
        });
        return format(formatString, argumentsStr);
    }
    babelwires::IdentifierRegistry::ReadAccess& m_identifierRegistry;
};

std::string babelwires::TypeRef::toStringHelper(babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const {
    const std::string str = std::visit(ToStringVisitor{identifierRegistry}, m_storage);
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
    std::vector<TypeRef> arguments;
    if (next == str.size()) {
        throw ParseException() << "Unterminated TypeRef";
    }
    while (1) {
        auto tuple = parseHelper(str.substr(next));
        arguments.emplace_back(std::move(std::get<0>(tuple)));
        if (arguments.size() > c_maxNumTypeConstructorArguments) {
            throw ParseException() << "TypeRef too many arguments (maximum allowed is "
                                   << c_maxNumTypeConstructorArguments << ")";
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
    switch (arguments.size()) {
        case 1:
            return {TypeRef{startId, std::move(arguments[0])}, next};
        case 2:
            return {TypeRef{startId, std::move(arguments[0]), std::move(arguments[1])}, next};
        default:
            throw ParseException() << "TypeRef with unsupported number of type arguments";
    }
}

babelwires::TypeRef babelwires::TypeRef::deserializeFromString(std::string_view str) {
    auto parseResult = parseHelper(str);
    if (std::get<1>(parseResult) != str.size()) {
        throw ParseException() << "Trailing characters in TypeRef";
    }
    return std::get<0>(parseResult);
}

struct babelwires::TypeRef::VisitIdentifiersVisitor {
    void operator()(std::monostate) {}
    void operator()(PrimitiveTypeId& typeId) { m_visitor(typeId); }
    template <unsigned int N> void operator()(ConstructedStorage<N>& higherOrderData) {
        m_visitor(higherOrderData->m_typeConstructorId);
        auto& arguments = higherOrderData->m_arguments.m_typeArguments;
        std::for_each(arguments.begin(), arguments.end(), [this](TypeRef& arg) { arg.visitIdentifiers(m_visitor); });
    }
    IdentifierVisitor& m_visitor;
};

void babelwires::TypeRef::visitIdentifiers(IdentifierVisitor& visitor) {
    // Note: The visitor needs to access the actual stored data, so be careful to avoid copies.
    return std::visit(VisitIdentifiersVisitor{visitor}, m_storage);
}

void babelwires::TypeRef::visitFilePaths(FilePathVisitor& visitor) {}

struct babelwires::TypeRef::GetHashVisitor {
    void operator()(std::monostate) { hash::mixInto(m_currentHash, 0x11122233); }
    void operator()(const PrimitiveTypeId& typeId) { hash::mixInto(m_currentHash, typeId); }
    template<unsigned int N>
    void operator()(const ConstructedStorage<N>& higherOrderData) {
        hash::mixInto(m_currentHash, higherOrderData->m_typeConstructorId, higherOrderData->m_arguments);
    }
    std::size_t& m_currentHash;
};

std::size_t babelwires::TypeRef::getHash() const {
    std::size_t hash = 0x123456789;
    // I wonder if the construction of std::hash objects creates pointless overhead here?
    std::visit(GetHashVisitor{hash}, m_storage);
    return hash;
}

struct babelwires::TypeRef::CompareSubtypeVisitor {
    SubtypeOrder operator()(std::monostate, std::monostate) { return SubtypeOrder::IsEquivalent; }
    SubtypeOrder operator()(const PrimitiveTypeId& typeIdA, const PrimitiveTypeId& typeIdB) {
        return m_typeSystem.compareSubtypePrimitives(typeIdA, typeIdB);
    }
    template <unsigned int N>
    SubtypeOrder operator()(const ConstructedStorage<N>& constructedTypeDataA, const PrimitiveTypeId& typeIdB) {
        const TypeConstructorId typeConstructorId = constructedTypeDataA->m_typeConstructorId;
        const TypeConstructor<N>* const typeConstructor = m_typeSystem.tryGetTypeConstructor<N>(typeConstructorId);
        if (!typeConstructor) {
            return SubtypeOrder::IsUnrelated;
        }
        return typeConstructor->compareSubtypeHelper(m_typeSystem, constructedTypeDataA->m_arguments, typeIdB);
    }
    template <unsigned int N>
    SubtypeOrder operator()(const PrimitiveTypeId& typeIdA, const ConstructedStorage<N>& constructedTypeDataB) {
        const TypeConstructorId typeConstructorIdB = constructedTypeDataB->m_typeConstructorId;
        const TypeConstructor<N>* const typeConstructorB = m_typeSystem.tryGetTypeConstructor<N>(typeConstructorIdB);
        if (!typeConstructorB) {
            return SubtypeOrder::IsUnrelated;
        }
        return reverseSubtypeOrder(
            typeConstructorB->compareSubtypeHelper(m_typeSystem, constructedTypeDataB->m_arguments, typeIdA));
    }
    template <unsigned int NA, unsigned int NB>
    SubtypeOrder operator()(const ConstructedStorage<NA>& constructedTypeDataA,
                            const ConstructedStorage<NB>& constructedTypeDataB) {
        const TypeConstructorId typeConstructorIdA = constructedTypeDataA->m_typeConstructorId;
        const TypeConstructorId typeConstructorIdB = constructedTypeDataB->m_typeConstructorId;
        const TypeConstructor<NA>* const typeConstructorA = m_typeSystem.tryGetTypeConstructor<NA>(typeConstructorIdA);
        if (!typeConstructorA) {
            return SubtypeOrder::IsUnrelated;
        }
        const auto& argsA = constructedTypeDataA->m_arguments;
        const auto& argsB = constructedTypeDataB->m_arguments;
        if constexpr (NA == NB) {
            if (typeConstructorIdA == typeConstructorIdB) {
                return typeConstructorA->compareSubtypeHelper(m_typeSystem, argsA, argsB);
            }
        }
        const TypeConstructor<NB>* const typeConstructorB = m_typeSystem.tryGetTypeConstructor<NB>(typeConstructorIdB);
        if (!typeConstructorB) {
            return SubtypeOrder::IsUnrelated;
        }
        auto comparisonUsingA = typeConstructorA->compareSubtypeHelper(m_typeSystem, argsA, m_typeRefB);
        if (comparisonUsingA != SubtypeOrder::IsUnrelated) {
            return comparisonUsingA;
        }
        // typeConstructorA doesn't know whether the types are related, but maybe typeConstructorB does.
        // For now, assume that there are no rules which require knowing typeConstructorA and typeConstructorB,
        // or rules where structure below the first level is needed.
        // Note that distributivity and associativity laws could violate these assumptions.
        return reverseSubtypeOrder(typeConstructorB->compareSubtypeHelper(m_typeSystem, argsB, m_typeRefA));
    }
    template <typename T, typename U> SubtypeOrder operator()(T&&, U&&) { return SubtypeOrder::IsUnrelated; }

    const TypeSystem& m_typeSystem;
    const TypeRef& m_typeRefA;
    const TypeRef& m_typeRefB;
};

babelwires::SubtypeOrder babelwires::TypeRef::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                   const TypeRef& typeRefA, const TypeRef& typeRefB) {
    return std::visit(CompareSubtypeVisitor{typeSystem, typeRefA, typeRefB}, typeRefA.m_storage, typeRefB.m_storage);
}
