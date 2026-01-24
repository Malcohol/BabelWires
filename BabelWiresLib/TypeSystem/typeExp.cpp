/**
 * A TypeExp describes a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeExp.hpp>

#include <BabelWiresLib/TypeSystem/Detail/typeNameFormatter.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

#include <bitset>

namespace {
    constexpr char defaultStateString[] = "[]";
} // namespace

std::size_t babelwires::TypeConstructorArguments::getHash() const {
    // Arbitrary value.
    std::size_t hash = 0x80235AA2;
    for (const auto& targ : m_typeArguments) {
        hash::mixInto(hash, targ);
    }
    for (const auto& varg : m_valueArguments) {
        hash::mixInto(hash, varg);
    }
    return hash;
}

bool babelwires::TypeConstructorArguments::equals(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
    return (a.m_typeArguments == b.m_typeArguments) && (a.m_valueArguments == b.m_valueArguments);
}

babelwires::TypeExp::TypeExp() = default;

babelwires::TypeExp::TypeExp(RegisteredTypeId typeId)
    : m_storage(typeId) {}

babelwires::TypeExp::TypeExp(TypeConstructorId typeConstructorId, TypeConstructorArguments arguments)
    : m_storage(ConstructedTypeData{typeConstructorId, std::move(arguments)}) {}

babelwires::TypePtr babelwires::TypeExp::tryResolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        TypePtr operator()(std::monostate) { return {}; }
        TypePtr operator()(RegisteredTypeId typeId) {
            return m_typeSystem.tryGetRegisteredTypeById(typeId);
        }
        TypePtr operator()(const ConstructedTypeData& higherOrderData) {
            try {
                const TypeConstructorId typeConstructorId = std::get<0>(higherOrderData);
                if (const TypeConstructor* const typeConstructor =
                        m_typeSystem.tryGetTypeConstructorById(typeConstructorId)) {
                    return typeConstructor->tryGetOrConstructType(m_typeSystem, std::get<1>(higherOrderData));
                }
                return {};
            } catch (babelwires::TypeSystemException&) {
                return {};
            }
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{typeSystem};
    return std::visit(visitorMethods, m_storage);
}

babelwires::TypePtr babelwires::TypeExp::resolve(const TypeSystem& typeSystem) const {
    struct VisitorMethods {
        TypePtr operator()(std::monostate) {
            throw TypeSystemException() << "A null type cannot be resolved.";
        }
        TypePtr operator()(RegisteredTypeId typeId) { return m_typeSystem.getRegisteredTypeById(typeId); }
        TypePtr operator()(const ConstructedTypeData& higherOrderData) {
            const TypeConstructorId typeConstructorId = std::get<0>(higherOrderData);
            const TypeConstructor& typeConstructor = m_typeSystem.getTypeConstructorById(typeConstructorId);
            return typeConstructor.getOrConstructType(m_typeSystem, std::get<1>(higherOrderData));
        }
        const TypeSystem& m_typeSystem;
    } visitorMethods{typeSystem};
    return std::visit(visitorMethods, m_storage);
}

babelwires::TypePtr babelwires::TypeExp::assertResolve(const TypeSystem& typeSystem) const {
#ifndef NDEBUG
    try {
#endif
        return resolve(typeSystem);
#ifndef NDEBUG
    } catch (TypeSystemException&) {
        assert(false && "TypeSystemException thrown when resolving TypeExp");
        return {};
    }
#endif
}

std::string babelwires::TypeExp::toStringHelper(babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const {
    struct VisitorMethods {
        std::string operator()(std::monostate) { return defaultStateString; }
        std::string operator()(RegisteredTypeId typeId) { return m_identifierRegistry->getName(typeId); }
        std::string operator()(const ConstructedTypeData& constructedTypeData) {
            std::string formatString = m_identifierRegistry->getName(std::get<0>(constructedTypeData));
            // TODO The VisitorMethods object should own these.
            std::vector<std::string> typeArgumentsStr;
            const auto& typeArguments = std::get<1>(constructedTypeData).getTypeArguments();
            std::for_each(typeArguments.begin(), typeArguments.end(),
                          [this, &typeArgumentsStr](const TypeExp& typeExp) {
                              // TODO Call visit, not toStringHelper.
                              typeArgumentsStr.emplace_back(typeExp.toStringHelper(m_identifierRegistry));
                          });
            std::vector<std::string> valueArgumentsStr;
            const auto& valueArguments = std::get<1>(constructedTypeData).getValueArguments();
            std::for_each(valueArguments.begin(), valueArguments.end(),
                          // TODO: value::toString should take the identifierRegistry as an argument.
                          [&valueArgumentsStr](const ValueHolder& value) {
                              valueArgumentsStr.emplace_back(value->getAsEditableValue().toString());
                          });
            return typeNameFormatter(formatString, typeArgumentsStr, valueArgumentsStr);
        }
        babelwires::IdentifierRegistry::ReadAccess& m_identifierRegistry;
    } visitorMethods{identifierRegistry};
    const std::string str = std::visit(visitorMethods, m_storage);
    if (!str.empty()) {
        return str;
    } else {
        return "MalformedTypeExp";
    }
}

std::string babelwires::TypeExp::toString() const {
    auto regScope = IdentifierRegistry::read();
    return toStringHelper(regScope);
}

void babelwires::TypeExp::serializeContents(Serializer& serializer) const {
    struct VisitorMethods {
        void operator()(std::monostate) {}
        void operator()(const RegisteredTypeId& typeId) { m_serializer.serializeValue("typeId", typeId); }
        void operator()(const ConstructedTypeData& constructedTypeData) {
            m_serializer.serializeValue("typeConstructorId", std::get<0>(constructedTypeData));
            m_serializer.serializeArray("typeArguments", std::get<1>(constructedTypeData).getTypeArguments());
            std::vector<const EditableValue*> tmpValueArray;
            std::for_each(std::get<1>(constructedTypeData).getValueArguments().begin(),
                          std::get<1>(constructedTypeData).getValueArguments().end(),
                          [&tmpValueArray](const ValueHolder& valueHolder) {
                              tmpValueArray.emplace_back(&valueHolder->getAsEditableValue());
                          });
            m_serializer.serializeArray("valueArguments", tmpValueArray);
        }
        Serializer& m_serializer;
    } visitorMethods{serializer};
    std::visit(visitorMethods, m_storage);
}

void babelwires::TypeExp::deserializeContents(Deserializer& deserializer) {
    RegisteredTypeId typeId;
    if (deserializer.deserializeValue("typeId", typeId, babelwires::Deserializer::IsOptional::Optional)) {
        m_storage = typeId;
    } else {
        TypeConstructorId typeConstructorId;
        if (deserializer.deserializeValue("typeConstructorId", typeConstructorId,
                                          babelwires::Deserializer::IsOptional::Optional)) {
            std::vector<TypeExp> typeArguments;
            std::vector<ValueHolder> valueArguments;
            auto typeIt = deserializer.deserializeArray<TypeExp>("typeArguments", Deserializer::IsOptional::Optional);
            while (typeIt.isValid()) {
                typeArguments.emplace_back(std::move(*typeIt.getObject()));
                ++typeIt;
            }
            auto valueIt =
                deserializer.deserializeArray<EditableValue>("valueArguments", Deserializer::IsOptional::Optional);
            while (valueIt.isValid()) {
                valueArguments.emplace_back(uniquePtrCast<Value>(valueIt.getObject()));
                ++valueIt;
            }
            m_storage = ConstructedTypeData{typeConstructorId, {std::move(typeArguments), std::move(valueArguments)}};
        } else {
            m_storage = {};
        }
    }
}

void babelwires::TypeExp::visitIdentifiers(IdentifierVisitor& visitor) {
    // Note: The visitor needs to access the actual stored data, so be careful to avoid copies.
    struct VisitorMethods {
        void operator()(std::monostate) {}
        void operator()(RegisteredTypeId& typeId) { m_visitor(typeId); }
        void operator()(ConstructedTypeData& higherOrderData) {
            m_visitor(std::get<0>(higherOrderData));
            auto& arguments = std::get<1>(higherOrderData).getTypeArguments();
            std::for_each(arguments.begin(), arguments.end(),
                          [this](TypeExp& arg) { arg.visitIdentifiers(m_visitor); });
        }
        IdentifierVisitor& m_visitor;
    } visitorMethods{visitor};
    return std::visit(visitorMethods, m_storage);
}

void babelwires::TypeExp::visitFilePaths(FilePathVisitor& visitor) {}

std::size_t babelwires::TypeExp::getHash() const {
    std::size_t hash = 0x123456789;
    // I wonder if the construction of std::hash objects creates pointless overhead here?
    struct VisitorMethods {
        void operator()(std::monostate) { hash::mixInto(m_currentHash, 0x11122233); }
        void operator()(const RegisteredTypeId& typeId) { hash::mixInto(m_currentHash, typeId); }
        void operator()(const ConstructedTypeData& higherOrderData) {
            hash::mixInto(m_currentHash, std::get<0>(higherOrderData), std::get<1>(higherOrderData));
        }
        std::size_t& m_currentHash;
    } visitorMethods{hash};
    std::visit(visitorMethods, m_storage);
    return hash;
}

babelwires::TypeExp::operator bool() const {
    return !std::holds_alternative<std::monostate>(m_storage);
}
