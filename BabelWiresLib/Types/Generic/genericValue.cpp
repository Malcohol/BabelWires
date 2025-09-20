/**
 *
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericValue.hpp>

#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/valuePathUtils.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>

babelwires::GenericValue::GenericValue(const TypeSystem& typeSystem, TypeRef wrappedType, unsigned int numVariables)
    : m_actualWrappedType(wrappedType)
    , m_typeVariableAssignments(numVariables)
    , m_wrappedValue(m_actualWrappedType.resolve(typeSystem).createValue(typeSystem)) {}

const babelwires::TypeRef& babelwires::GenericValue::getActualWrappedType() const {
    return m_actualWrappedType;
}

const std::vector<babelwires::TypeRef>& babelwires::GenericValue::getTypeAssignments() const {
    return m_typeVariableAssignments;
}

std::vector<babelwires::TypeRef>& babelwires::GenericValue::getTypeAssignments() {
    return m_typeVariableAssignments;
}

const babelwires::ValueHolder& babelwires::GenericValue::getValue() const {
    return m_wrappedValue;
}

babelwires::ValueHolder& babelwires::GenericValue::getValue() {
    return m_wrappedValue;
}

std::size_t babelwires::GenericValue::getHash() const {
    auto hash = hash::mixtureOf(std::string("Generic"), m_actualWrappedType, m_wrappedValue);
    for (const auto& t : m_typeVariableAssignments) {
        hash::mixInto(hash, t);
    }
    return hash;
}

bool babelwires::GenericValue::operator==(const Value& other) const {
    if (const GenericValue* otherValue = other.as<GenericValue>()) {
        return (m_actualWrappedType == otherValue->m_actualWrappedType) &&
               (m_wrappedValue == otherValue->m_wrappedValue);
    } else {
        return false;
    }
}

babelwires::TypeRef babelwires::GenericValue::buildInstantiatedType(const TypeRef& wrappedType) const {
    struct Visitor {
        Visitor(const GenericValue& genericValue, unsigned int level = 0)
            : m_genericValue(genericValue)
            , m_level(level) {}
        TypeRef operator()(std::monostate) { return TypeRef(); }
        TypeRef operator()(const RegisteredTypeId& typeId) {
            // Simplifying restriction for now: registered types may not contain unbound type variables.
            return typeId;
        }
        TypeRef operator()(const TypeConstructorId& constructorId,
                           const TypeConstructorArguments& constructorArguments) {
            if (constructorId == TypeVariableTypeConstructor::getThisIdentifier()) {
                const TypeVariableData variableData =
                    TypeVariableTypeConstructor::extractValueArguments(constructorArguments.m_valueArguments);
                if (variableData.m_numGenericTypeLevels == m_level) {
                    const auto& typeAssignments = m_genericValue.m_typeVariableAssignments;
                    assert(variableData.m_typeVariableIndex <= typeAssignments.size());
                    if (const TypeRef& assignment = typeAssignments[variableData.m_typeVariableIndex]) {
                        return TypeRef(constructorId, {{assignment}, constructorArguments.m_valueArguments});
                    } else {
                        return TypeRef(constructorId, constructorArguments);
                    }
                }
            }
            unsigned int level = m_level;
            if (constructorId == GenericTypeConstructor::getThisIdentifier()) {
                // Variables under this constructor would have to jump over this GenericType to
                // reach the one were processing.
                ++level;
            }
            std::vector<TypeRef> children;
            children.reserve(constructorArguments.m_typeArguments.size());
            for (const auto& c : constructorArguments.m_typeArguments) {
                Visitor childVisitor(m_genericValue, level);
                children.emplace_back(c.visit<Visitor, TypeRef>(childVisitor));
            }
            return TypeRef(constructorId, TypeConstructorArguments{children, constructorArguments.m_valueArguments});
        }
        const GenericValue& m_genericValue;
        unsigned int m_level;
    } visitor(*this);
    return wrappedType.visit<Visitor, TypeRef>(visitor);
}

void babelwires::GenericValue::instantiate(const TypeSystem& typeSystem, const TypeRef& wrappedTypeRef) {
    m_actualWrappedType = buildInstantiatedType(wrappedTypeRef);
    // Updating the existing wrapped value by exploring it doesn't account for the fact that values may carry types in
    // non-obvious ways. In particular, GenericValue carries the m_actualWrappedType and typeAssignments, neither of
    // which would be updated if we encountered a nested generic type using exploration.
    m_wrappedValue = m_actualWrappedType.resolve(typeSystem).createValue(typeSystem);
}
