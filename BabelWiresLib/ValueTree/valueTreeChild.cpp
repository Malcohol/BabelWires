/**
 * A ValueTreeChild is a ValueTreeNode whose value is owned by an ancestor feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreeChild.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

babelwires::ValueTreeChild::ValueTreeChild(TypeRef typeRef, const ValueHolder& valueHolder, ValueTreeNode* owner)
    : ValueTreeNode(std::move(typeRef), valueHolder) {
    assert(owner != nullptr);
    setOwner(owner);
}

void babelwires::ValueTreeChild::doSetValue(const ValueHolder& newValue) {
    const ValueHolder& currentValue = getValue();
    if (currentValue != newValue) {
        const TypeSystem& typeSystem = getTypeSystem();
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            auto rootAndPath = getRootAndPathTo(*this);
            rootAndPath.m_root.setDescendentValue(rootAndPath.m_pathFromRoot, newValue);
        } else {
            throw ModelException() << "The new value is not a valid instance of " << getTypeRef().toString();
        }
    }
}

void babelwires::ValueTreeChild::doSetToDefault() {
    const TypeSystem& typeSystem = getTypeSystem();
    const auto [newValue, _] = getType().createValue(typeSystem);
    const auto rootAndPath = getRootAndPathTo(*this);
    rootAndPath.m_root.setDescendentValue(rootAndPath.m_pathFromRoot, newValue);
}

void babelwires::ValueTreeChild::setType(const TypeRef& type) {
    const auto variableData = TypeVariableTypeConstructor::isTypeVariable(getTypeRef());
    if (!variableData) {
        return;
    }

    // Search up the tree for the generic type.
    unsigned int level = variableData->m_numGenericTypeLevels;
    ValueTreeNode* current = this;
    ValueTreeNode* parent;
    do {
        parent = current->getOwnerNonConst();
        if (!parent) {
            // This could happen if a subtree beneath a generic type was dragged out of a node,
            // TODO: This is not a useful state, so do something to prevent it.
            return;
        } else if (parent->getType().as<GenericType>()) {
            if (level == 0) {
                current = parent;
                break;
            } else {
                --level;
            }
        }
        current = parent;
    } while (1);
    const GenericType& genericType = current->getType().is<GenericType>();

    const TypeSystem& typeSystem = getTypeSystem();

    // Get a non-const ValueHolder corresponding to current.
    ValueHolder newValue = current->getValue();
    genericType.instantiate(typeSystem, newValue, variableData->m_typeVariableIndex, type);

    // Set this value at the tree
    const auto rootAndPath = getRootAndPathTo(*current);

    rootAndPath.m_root.setDescendentValue(rootAndPath.m_pathFromRoot, newValue);
}