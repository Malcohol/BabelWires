/**
 * A ChildValueFeature is a Feature whose value is owned by an ancestor feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/childValueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Features/Path/path.hpp>

babelwires::ChildValueFeature::ChildValueFeature(TypeRef typeRef, const ValueHolder* valueHolder)
    : Feature(std::move(typeRef))
    , m_value(valueHolder) {
    assert(valueHolder);
}

void babelwires::ChildValueFeature::ensureSynchronized(const ValueHolder* valueHolder) {
    assert(valueHolder);
    if (m_value != valueHolder) {
        m_value = valueHolder;
        synchronizeSubfeatures();
    }
}

const babelwires::ValueHolder& babelwires::ChildValueFeature::doGetValue() const {
    return *m_value;
}

void babelwires::ChildValueFeature::doSetValue(const ValueHolder& newValue) {
    const ValueHolder& currentValue = doGetValue();
    if (currentValue != newValue) {
        const TypeSystem& typeSystem = getTypeSystem();
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            auto rootAndPath = Path::getRootAndPath(*this);
            ValueHolder& modifiableValueHolder = rootAndPath.m_root.setModifiable(rootAndPath.m_pathFromRoot);
            modifiableValueHolder = newValue;
            // Changing the modifiableValueHolder, can change the value.
            synchronizeSubfeatures();
        } else {
            throw ModelException() << "The new value is not a valid instance of " << getTypeRef().toString();
        }
    }
}

void babelwires::ChildValueFeature::doSetToDefault() {
    const TypeSystem& typeSystem = getTypeSystem();
    auto [newValue, _] = getType().createValue(typeSystem);
    auto rootAndPath = Path::getRootAndPath(*this);
    ValueHolder& modifiableValueHolder = rootAndPath.m_root.setModifiable(rootAndPath.m_pathFromRoot);
    modifiableValueHolder = newValue;
    // Changing the modifiableValueHolder can change the structure of the hierarchy at this point.
    synchronizeSubfeatures();
}
