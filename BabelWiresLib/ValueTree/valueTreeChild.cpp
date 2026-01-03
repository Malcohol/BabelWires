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
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

babelwires::ValueTreeChild::ValueTreeChild(TypePtr typePtr, const ValueHolder& valueHolder, ValueTreeNode* owner)
    : ValueTreeNode(std::move(typePtr), valueHolder) {
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
            throw ModelException() << "The new value is not a valid instance of " << getTypeExp().toString();
        }
    }
}

void babelwires::ValueTreeChild::doSetToDefault() {
    const TypeSystem& typeSystem = getTypeSystem();
    const auto [newValue, _] = getType().createValue(typeSystem);
    const auto rootAndPath = getRootAndPathTo(*this);
    rootAndPath.m_root.setDescendentValue(rootAndPath.m_pathFromRoot, newValue);
}
