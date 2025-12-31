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
    , m_wrappedValue(m_actualWrappedType.resolve(typeSystem)->createValue(typeSystem)) {}

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
                    TypeVariableTypeConstructor::extractValueArguments(constructorArguments.getValueArguments());
                if (variableData.m_numGenericTypeLevels == m_level) {
                    const auto& typeAssignments = m_genericValue.m_typeVariableAssignments;
                    assert(variableData.m_typeVariableIndex <= typeAssignments.size());
                    if (const TypeRef& assignment = typeAssignments[variableData.m_typeVariableIndex]) {
                        return TypeRef(constructorId, {{assignment}, constructorArguments.getValueArguments()});
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
            children.reserve(constructorArguments.getTypeArguments().size());
            for (const auto& c : constructorArguments.getTypeArguments()) {
                Visitor childVisitor(m_genericValue, level);
                children.emplace_back(c.visit<Visitor, TypeRef>(childVisitor));
            }
            return TypeRef(constructorId, TypeConstructorArguments{children, constructorArguments.getValueArguments()});
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
    m_wrappedValue = m_actualWrappedType.resolve(typeSystem)->createValue(typeSystem);
}

bool babelwires::GenericValue::isActualVersionOf(const TypeRef& wrappedType) const {
    struct Visitor {
        Visitor(const GenericValue& genericValue, unsigned int level = 0)
            : m_genericValue(genericValue)
            , m_level(level) {}
        bool operator()() { return false; } // Called when structurally different
        bool operator()(std::monostate, std::monostate) { return true; }
        bool operator()(const RegisteredTypeId& typeIdGen, const RegisteredTypeId& typeIdAct) {
            // Simplifying restriction for now: registered types may not contain unbound type variables.
            return typeIdGen == typeIdAct;
        }
        bool operator()(const TypeConstructorId& constructorIdRef,
                        const TypeConstructorArguments& constructorArgumentsRef,
                        const TypeConstructorId& constructorIdAct,
                        const TypeConstructorArguments& constructorArgumentsAct) {
            if (constructorIdRef != constructorIdAct) {
                return false;
            }
            if (constructorIdRef == TypeVariableTypeConstructor::getThisIdentifier()) {
                const TypeVariableData variableDataRef =
                    TypeVariableTypeConstructor::extractValueArguments(constructorArgumentsRef.getValueArguments());
                const TypeVariableData variableDataAct =
                    TypeVariableTypeConstructor::extractValueArguments(constructorArgumentsAct.getValueArguments());
                if ((variableDataRef.m_typeVariableIndex != variableDataAct.m_typeVariableIndex) ||
                    (variableDataRef.m_numGenericTypeLevels != variableDataAct.m_numGenericTypeLevels)) {
                    return false;
                }
                if (variableDataAct.m_numGenericTypeLevels == m_level) {
                    assert(variableDataAct.m_typeVariableIndex <= m_genericValue.m_typeVariableAssignments.size());
                    assert(!m_genericValue.m_typeVariableAssignments[variableDataAct.m_typeVariableIndex] ||
                           (constructorArgumentsAct.getTypeArguments().size() == 1));
                    assert(!m_genericValue.m_typeVariableAssignments[variableDataAct.m_typeVariableIndex] ||
                           (m_genericValue.m_typeVariableAssignments[variableDataAct.m_typeVariableIndex] ==
                            constructorArgumentsAct.getTypeArguments()[0]));
                    // Allowed to differ here.
                    return true;
                }
            }
            unsigned int level = m_level;
            if (constructorIdRef == GenericTypeConstructor::getThisIdentifier()) {
                // Variables under this constructor would have to jump over this GenericType to
                // reach the one were processing.
                ++level;
            }
            // Compare the type and value arguments.
            if ((constructorArgumentsRef.getTypeArguments().size() !=
                 constructorArgumentsAct.getTypeArguments().size()) ||
                (constructorArgumentsRef.getValueArguments().size() !=
                 constructorArgumentsAct.getValueArguments().size())) {
                return false;
            }
            for (unsigned int i = 0; i < constructorArgumentsAct.getValueArguments().size(); ++i) {
                if (constructorArgumentsRef.getValueArguments()[i] != constructorArgumentsAct.getValueArguments()[i]) {
                    return false;
                }
            }
            for (unsigned int i = 0; i < constructorArgumentsAct.getTypeArguments().size(); ++i) {
                Visitor childVisitor(m_genericValue, level);
                if (!TypeRef::visit<Visitor, bool>(childVisitor, constructorArgumentsRef.getTypeArguments()[i],
                                                   constructorArgumentsAct.getTypeArguments()[i])) {
                    return false;
                }
            }
            return true;
        }
        const GenericValue& m_genericValue;
        unsigned int m_level;
    } visitor(*this);
    return TypeRef::visit<Visitor, bool>(visitor, wrappedType, m_actualWrappedType);
}
