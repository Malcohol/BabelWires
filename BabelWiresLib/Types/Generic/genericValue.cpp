/**
 * 
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Generic/genericValue.hpp>

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>

babelwires::GenericValue::GenericValue(const TypeSystem& typeSystem, TypeRef wrappedType, unsigned int numVariables)
    : m_actualWrappedType(wrappedType)
    , m_typeVariableAssignments(numVariables)
    , m_wrappedValue(m_actualWrappedType.resolve(typeSystem).createValue(typeSystem))
{
}


const babelwires::TypeRef& babelwires::GenericValue::getWrappedType() const {
    return m_actualWrappedType;
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
        return (m_actualWrappedType == otherValue->m_actualWrappedType) && (m_wrappedValue == otherValue->m_wrappedValue);
    } else {
        return false;
    }
}

void babelwires::GenericValue::assignTypeVariable(unsigned int variableIndex, const TypeRef& typeValue) {
    assert(variableIndex < m_typeVariableAssignments.size());
    m_typeVariableAssignments[variableIndex] = typeValue;
}

babelwires::TypeRef babelwires::GenericValue::buildInstantiatedType(const TypeRef& wrappedType) const {
    struct Visitor {
        Visitor(const GenericValue& genericValue, unsigned int level = 0) : m_genericValue(genericValue), m_level(level) {}
        TypeRef operator()(std::monostate) {
            return TypeRef();
        }
        TypeRef operator()(const RegisteredTypeId& typeId) { 
            // Simplifying restriction for now: registered types may not contain unbound type variables.
            return typeId;
        }
        TypeRef operator()(const TypeConstructorId& constructorId, const TypeConstructorArguments& constructorArguments) {
            if (constructorId == TypeVariableTypeConstructor::getThisIdentifier()) {
                const TypeVariableTypeConstructor::VariableData variableData = TypeVariableTypeConstructor::extractValueArguments(constructorArguments.m_valueArguments);
                if (variableData.m_numGenericTypeLevels == m_level) {
                    const auto& typeAssignments = m_genericValue.m_typeVariableAssignments;
                    assert(variableData.m_typeVariableIndex <= typeAssignments.size());
                    if (const TypeRef& assignment = typeAssignments[variableData.m_typeVariableIndex]) {
                        return assignment;
                    } else {
                        return TypeRef(constructorId, constructorArguments);
                    }
                }
            }
            unsigned int level = m_level;
            // TODO when GenericTypeConstructor is implemented, special case it here to increment level.
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

void babelwires::GenericValue::instantiate(const TypeSystem& typeSystem, const TypeRef& wrappedType) {
    m_actualWrappedType = buildInstantiatedType(wrappedType);
    m_wrappedValue = m_actualWrappedType.resolve(typeSystem).createValue(typeSystem);
}
