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

babelwires::TypeRef::TypeRef() = default;

babelwires::TypeRef::TypeRef(PrimitiveTypeId typeId)
    : m_storage(typeId) {}

babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, Arguments arguments)
    : m_storage(ConstructedTypeData{typeConstructorId, std::move(arguments)}) {
    assert((arguments.size() <= s_maxNumArguments) && "Too many arguments for TypeRef");
}

const babelwires::Type* babelwires::TypeRef::tryResolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        const babelwires::Type* operator()(std::monostate) { return nullptr; }
        const babelwires::Type* operator()(PrimitiveTypeId typeId) { return m_typeSystem.tryGetPrimitiveType(typeId); }
        const babelwires::Type* operator()(const ConstructedTypeData& higherOrderData) {
            // TODO
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
            // TODO
            throw TypeSystemException() << "TODO";
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{typeSystem};
    return std::visit(visitorMethods, m_storage);
}

std::string babelwires::TypeRef::serializeToString() const {
    struct VisitorMethods {
        std::string operator()(std::monostate) { return "<>"; }
        std::string operator()(PrimitiveTypeId typeId) { return typeId.serializeToString(); }
        std::string operator()(const ConstructedTypeData& higherOrderData) {
            std::ostringstream os;
            os << std::get<0>(higherOrderData).serializeToString();
            const auto& arguments = std::get<1>(higherOrderData);
            if (arguments.empty()) {
                // Correctly formed TypeRef should not hit this case.
                // It can be used when falling-back to this representation
                // from toString.
                os << "<>";
            } else {
                char sep = '<';
                for (const auto& arg : arguments) {
                    os << sep << arg.serializeToString();
                    sep = ',';
                }
                os << ">";
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
        if (arguments.size() > babelwires::TypeRef::s_maxNumArguments) {
            return {};
        }
        std::bitset<babelwires::TypeRef::s_maxNumArguments> argumentsNotYetSeen((1 << arguments.size()) - 1);
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
        std::string operator()(std::monostate) { return "<>"; }
        std::string operator()(PrimitiveTypeId typeId) { return m_identifierRegistry->getName(typeId); }
        std::string operator()(const ConstructedTypeData& higherOrderData) {
            std::string formatString = m_identifierRegistry->getName(std::get<0>(higherOrderData));
            std::vector<std::string> argumentsStr;
            const auto& arguments = std::get<1>(higherOrderData);
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
    auto IdEnd = str.find_first_of("<>,", next);
    if (IdEnd == -1) {
        IdEnd = str.size();
    }
    if ((IdEnd + 1 < str.size()) && (IdEnd == next) && (str[IdEnd] == '<') && (str[IdEnd + 1] == '>')) {
        return {TypeRef(), IdEnd + 2};
    }
    const LongIdentifier startId = LongIdentifier::deserializeFromString(str.substr(next, IdEnd));
    next = IdEnd;
    if (str[next] != '<') {
        return {startId, next};
    }
    ++next;
    Arguments arguments;
    if (next == str.size()) {
        throw ParseException() << "Unterminated TypeRef";
    }
    while (1) {
        auto tuple = parseHelper(str.substr(next));
        arguments.emplace_back(std::move(std::get<0>(tuple)));
        if (arguments.size() > s_maxNumArguments) {
            throw ParseException() << "TypeRef too many arguments (maximum allowed is " << s_maxNumArguments << ")";
        }
        assert((std::get<1>(tuple) > 0) && "Did not advance while parsing");
        next += std::get<1>(tuple);
        if (next == str.size()) {
            throw ParseException() << "Unterminated TypeRef";
        }
        if (str[next] == ',') {
            ++next;
        } else if (str[next] == '>') {
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
            Arguments& arguments = std::get<1>(higherOrderData);
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
            hash::mixInto(m_currentHash, std::get<0>(higherOrderData));
            const Arguments& arguments = std::get<1>(higherOrderData);
            std::for_each(arguments.cbegin(), arguments.cend(),
                          [this](const TypeRef& arg) { hash::mixInto(m_currentHash, arg); });
        }
        std::size_t& m_currentHash;
    } visitorMethods{hash};
    std::visit(visitorMethods, m_storage);
    return hash;
}
