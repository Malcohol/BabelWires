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
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>

babelwires::GenericValue::GenericValue(const TypeSystem& typeSystem, TypeRef wrappedType, unsigned int numVariables)
    : m_actualWrappedType(wrappedType)
    , m_typeVariableAssignments(numVariables)
    , m_wrappedValue(m_actualWrappedType.resolve(typeSystem).createValue(typeSystem)) {}

const babelwires::TypeRef& babelwires::GenericValue::getWrappedType() const {
    return m_actualWrappedType;
}

const std::vector<babelwires::TypeRef>& babelwires::GenericValue::getTypeAssignments() const {
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
                const TypeVariableTypeConstructor::VariableData variableData =
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

namespace babelwires {

    void findPathsRecursive(const TypeSystem& typeSystem, const Type& type, const ValueHolder& value, unsigned int variableIndex,
                           unsigned int genericTypeCount, const Path& pathToHere, std::vector<Path>& pathsOut) {
        if (const auto& typeVar = type.as<TypeVariableType>()) {
            const TypeVariableTypeConstructor::VariableData variableData = typeVar->getVariableData();
            if ((genericTypeCount == variableData.m_numGenericTypeLevels) &&
                (variableIndex == variableData.m_typeVariableIndex)) {
                    pathsOut.emplace_back(pathToHere);
            }
        } else if (const auto& compoundType = type.as<CompoundType>()) {
            if (type.as<GenericType>()) {
                ++genericTypeCount;
            }
            for (unsigned int i = 0; i < compoundType->getNumChildren(value); ++i) {
                auto [childValue, step, childTypeRef] = compoundType->getChild(value, i);
                const Type& childType = childTypeRef.resolve(typeSystem);
                Path pathToChild = pathToHere;
                pathToChild.pushStep(step);
                findPathsRecursive(typeSystem, childType, *childValue, variableIndex, genericTypeCount, pathToChild, pathsOut);
            }
        } else {
            return;
        }
    }

} // namespace babelwires

std::vector<babelwires::Path>
babelwires::GenericValue::findPathsToTypeVariableInstances(const TypeSystem& typeSystem, unsigned int variableIndex) const {
    assert((variableIndex < m_typeVariableAssignments.size()) && "variableIndex out of range");
    assert(!m_typeVariableAssignments[variableIndex] && "The  type variable at variableIndex is already instantiated");
    const Type& type = m_actualWrappedType.resolve(typeSystem);
    std::vector<babelwires::Path> pathsOut;
    findPathsRecursive(typeSystem, type, m_wrappedValue, variableIndex, 0, Path(), pathsOut);
    return pathsOut;
}

void babelwires::GenericValue::assignTypeVariableAndInstantiate(const TypeSystem& typeSystem, const TypeRef& wrappedTypeRef, unsigned int variableIndex, const TypeRef& typeValue) {
    assert((variableIndex < m_typeVariableAssignments.size()) && "variableIndex out of range");
    assert(!m_typeVariableAssignments[variableIndex] && "The  type variable at variableIndex is already instantiated");

    const std::vector<Path> pathsToTypeVariables = findPathsToTypeVariableInstances(typeSystem, variableIndex);
    const Type& wrappedType = m_actualWrappedType.resolve(typeSystem);
    const Type& typeValueType = typeValue.resolve(typeSystem);
    visitPathsNonConst(typeSystem, wrappedType, m_wrappedValue, pathsToTypeVariables, [&typeSystem, &typeValueType](const Type& type, ValueHolder& typeVariableValue) {
        typeVariableValue = typeValueType.createValue(typeSystem);
    });

    m_typeVariableAssignments[variableIndex] = typeValue;
    m_actualWrappedType = buildInstantiatedType(wrappedTypeRef);
}
