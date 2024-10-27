/**
 * A ValueTreeChild is a ValueTreeNode whose value is owned by an ancestor feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreeChild.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Path/path.hpp>

babelwires::ValueTreeChild::ValueTreeChild(TypeRef typeRef, const ValueHolder* valueHolder)
    : ValueTreeNode(std::move(typeRef))
    , m_value(valueHolder) {
    assert(valueHolder);
}

void babelwires::ValueTreeChild::ensureSynchronized(const ValueHolder* valueHolder) {
    assert(valueHolder);
    if (m_value != valueHolder) {
        m_value = valueHolder;
        synchronizeChildren();
    }
}

const babelwires::ValueHolder& babelwires::ValueTreeChild::doGetValue() const {
    return *m_value;
}

void babelwires::ValueTreeChild::doSetValue(const ValueHolder& newValue) {
    const ValueHolder& currentValue = doGetValue();
    if (currentValue != newValue) {
        const TypeSystem& typeSystem = getTypeSystem();
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            auto rootAndPath = Path::getRootAndPath(*this);
            ValueHolder& modifiableValueHolder = rootAndPath.m_root.setModifiable(rootAndPath.m_pathFromRoot);
            modifiableValueHolder = newValue;
            // Changing the modifiableValueHolder, can change the value.
            synchronizeChildren();
        } else {
            throw ModelException() << "The new value is not a valid instance of " << getTypeRef().toString();
        }
    }
}

void babelwires::ValueTreeChild::doSetToDefault() {
    const TypeSystem& typeSystem = getTypeSystem();
    auto [newValue, _] = getType().createValue(typeSystem);
    auto rootAndPath = Path::getRootAndPath(*this);
    ValueHolder& modifiableValueHolder = rootAndPath.m_root.setModifiable(rootAndPath.m_pathFromRoot);
    modifiableValueHolder = newValue;
    // Changing the modifiableValueHolder can change the structure of the hierarchy at this point.
    synchronizeChildren();
}
