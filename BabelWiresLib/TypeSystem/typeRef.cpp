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
#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

#include <bitset>

namespace {
    constexpr char defaultStateString[] = "[]";
} // namespace

babelwires::TypeRef::TypeRef() = default;

babelwires::TypeRef::TypeRef(PrimitiveTypeId typeId)
    : m_storage(typeId) {}

babelwires::TypeRef::TypeRef(TypeConstructorId typeConstructorId, TypeConstructorArguments arguments)
    : m_storage(ConstructedTypeData{typeConstructorId, std::move(arguments)}) {
    assert((arguments.m_typeArguments.size() <= TypeConstructorArguments::s_maxNumArguments) &&
           "Too many arguments for TypeRef");
}

const babelwires::Type* babelwires::TypeRef::tryResolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        const babelwires::Type* operator()(std::monostate) { return nullptr; }
        const babelwires::Type* operator()(PrimitiveTypeId typeId) { return m_typeSystem.tryGetPrimitiveType(typeId); }
        const babelwires::Type* operator()(const ConstructedTypeData& higherOrderData) {
            try {
                const TypeConstructorId typeConstructorId = std::get<0>(higherOrderData);
                if (const TypeConstructor* const typeConstructor =
                        m_typeSystem.tryGetTypeConstructor(typeConstructorId)) {
                    return typeConstructor->tryGetOrConstructType(m_typeSystem, std::get<1>(higherOrderData));
                }
                return nullptr;
            } catch (babelwires::TypeSystemException&) {
                return nullptr;
            }
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
            const TypeConstructor& typeConstructor = m_typeSystem.getTypeConstructor(typeConstructorId);
            return typeConstructor.getOrConstructType(m_typeSystem, std::get<1>(higherOrderData));
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{typeSystem};
    return std::visit(visitorMethods, m_storage);
}

const babelwires::Type& babelwires::TypeRef::assertResolve(const TypeSystem& typeSystem) const {
#ifndef NDEBUG
    try {
#endif
        return resolve(typeSystem);
#ifndef NDEBUG
    } catch (TypeSystemException&) {
        assert(false && "TypeSystemException thrown when resolving TypeRef");
    }
#endif
}

namespace {
    /// A very simple string formatting algorithm sufficient for this job.
    /// Hopefully could be replaced by std::format when that's properly supported.
    std::string simpleFormat(std::string_view format, const std::vector<std::string>& arguments) {
        if (format.size() < 2) {
            return {};
        }
        if (arguments.size() > babelwires::TypeConstructorArguments::s_maxNumArguments) {
            return {};
        }
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
                    static_assert(babelwires::TypeConstructorArguments::s_maxNumArguments == 10);
                    if ((currentChar >= '0') && (currentChar <= '9')) {
                        const std::size_t indexCharAsIndex = currentChar - '0';
                        if (indexCharAsIndex >= arguments.size()) {
                            return {};
                        }
                        oss << arguments[indexCharAsIndex];
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
        if (state == normal) {
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
        std::string operator()(const ConstructedTypeData& constructedTypeData) {
            std::string formatString = m_identifierRegistry->getName(std::get<0>(constructedTypeData));
            std::vector<std::string> argumentsStr;
            const auto& typeArguments = std::get<1>(constructedTypeData).m_typeArguments;
            std::for_each(typeArguments.begin(), typeArguments.end(), [this, &argumentsStr](const TypeRef& typeRef) {
                argumentsStr.emplace_back(typeRef.toStringHelper(m_identifierRegistry));
            });
            const auto& valueArguments = std::get<1>(constructedTypeData).m_valueArguments;
            std::for_each(valueArguments.begin(), valueArguments.end(),
                          [&argumentsStr](const EditableValueHolder& value) { argumentsStr.emplace_back(value->toString()); });
            return simpleFormat(formatString, argumentsStr);
        }
        babelwires::IdentifierRegistry::ReadAccess& m_identifierRegistry;
    } visitorMethods{identifierRegistry};
    const std::string str = std::visit(visitorMethods, m_storage);
    if (!str.empty()) {
        return str;
    } else {
        return "MalformedTypeRef";
    }
}

std::string babelwires::TypeRef::toString() const {
    auto regScope = IdentifierRegistry::read();
    return toStringHelper(regScope);
}

void babelwires::TypeRef::serializeContents(Serializer& serializer) const {
    struct VisitorMethods {
        void operator()(std::monostate) {}
        void operator()(const PrimitiveTypeId& typeId) { m_serializer.serializeValue("primitiveTypeId", typeId); }
        void operator()(const ConstructedTypeData& constructedTypeData) {
            m_serializer.serializeValue("typeConstructorId", std::get<0>(constructedTypeData));
            m_serializer.serializeArray("typeArguments", std::get<1>(constructedTypeData).m_typeArguments);
            std::vector<const EditableValue*> tmpValueArray;
            std::for_each(std::get<1>(constructedTypeData).m_valueArguments.begin(),
                          std::get<1>(constructedTypeData).m_valueArguments.end(),
                          [&tmpValueArray](const EditableValueHolder& valueHolder) {
                              tmpValueArray.emplace_back(valueHolder.getUnsafe());
                          });
            m_serializer.serializeArray("valueArguments", tmpValueArray);
        }
        Serializer& m_serializer;
    } visitorMethods{serializer};
    std::visit(visitorMethods, m_storage);
}

void babelwires::TypeRef::deserializeContents(Deserializer& deserializer) {
    PrimitiveTypeId primitiveTypeId;
    if (deserializer.deserializeValue("primitiveTypeId", primitiveTypeId,
                                      babelwires::Deserializer::IsOptional::Optional)) {
        m_storage = primitiveTypeId;
    } else {
        TypeConstructorId typeConstructorId;
        if (deserializer.deserializeValue("typeConstructorId", typeConstructorId,
                                          babelwires::Deserializer::IsOptional::Optional)) {
            TypeConstructorArguments arguments;
            auto typeIt = deserializer.deserializeArray<TypeRef>("typeArguments", Deserializer::IsOptional::Optional);
            while (typeIt.isValid()) {
                arguments.m_typeArguments.emplace_back(std::move(*typeIt.getObject()));
                ++typeIt;
            }
            auto valueIt =
                deserializer.deserializeArray<EditableValue>("valueArguments", Deserializer::IsOptional::Optional);
            while (valueIt.isValid()) {
                arguments.m_valueArguments.emplace_back(valueIt.getObject());
                ++valueIt;
            }
            m_storage = ConstructedTypeData{typeConstructorId, std::move(arguments)};
        } else {
            m_storage = {};
        }
    }
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

babelwires::TypeRef::operator bool() const {
    return !std::holds_alternative<std::monostate>(m_storage);
}
