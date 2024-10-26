/**
 * A SimpleValueFeature is a Feature which owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/valuePath.hpp>

babelwires::SimpleValueFeature::SimpleValueFeature(const TypeSystem& typeSystem, TypeRef typeRef)
    : Feature(std::move(typeRef))
    , m_typeSystem(&typeSystem) {
    // TODO assert the type resolves?
}

const babelwires::ValueHolder& babelwires::SimpleValueFeature::doGetValue() const {
    // Not sure if this assert is necessary.
    assert(m_value && "The SimpleValueFeature has not been initialized");
    return m_value;
}

void babelwires::SimpleValueFeature::doSetValue(const ValueHolder& newValue) {
    if (m_value != newValue) {
        const TypeSystem& typeSystem = getTypeSystem();
        const Type& type = getType();
        if (type.isValidValue(typeSystem, *newValue)) {
            ValueHolder backup = m_value;
            m_value = newValue;
            synchronizeSubfeatures();
            if (backup) {
                reconcileChanges(backup);
            } else {
                setChanged(type.as<CompoundType>() ? Changes::StructureChanged : Changes::ValueChanged);
            }
        } else {
            throw ModelException() << "The new value is not a valid instance of " << getTypeRef().toString();
        }
    }
}

void babelwires::SimpleValueFeature::doSetToDefault() {
    assert(getTypeRef() && "The type must be set to something non-trivial before doSetToDefault is called");
    const TypeSystem& typeSystem = getTypeSystem();
    const Type& type = getType();
    auto [newValue, _] = type.createValue(typeSystem);
    if (m_value != newValue) {
        m_value.swap(newValue);
        synchronizeSubfeatures();
        if (newValue) {
            reconcileChanges(newValue);
        } else {
            setChanged(type.as<CompoundType>() ? Changes::StructureChanged : Changes::ValueChanged);
        }
    }
}

void babelwires::SimpleValueFeature::backUpValue() {
    assert(!m_valueBackUp && "The value is already backed-up");
    m_valueBackUp = m_value;
}

babelwires::ValueHolder& babelwires::SimpleValueFeature::setModifiable(const Path& pathFromHere) {
    if (pathFromHere.getNumSteps() > 0) {
        assert(getType().as<CompoundType>() && "Path leading into a non-compound type");
        assert(m_isNew ||
               m_valueBackUp && "You cannot make a feature modifiable if its RootValueFeature has not been backed up");
        const TypeSystem& typeSystem = getTypeSystem();
        auto [_, valueInCopy] = followNonConst(typeSystem, getType(), pathFromHere, m_value);
        synchronizeSubfeatures();
        return valueInCopy;
    } else {
        return m_value;
    }
}

void babelwires::SimpleValueFeature::reconcileChangesFromBackup() {
    if (m_valueBackUp) {
        reconcileChanges(m_valueBackUp);
        m_valueBackUp.clear();
    }
    m_isNew = false;
}

const babelwires::TypeSystem& babelwires::SimpleValueFeature::getTypeSystem() const {
    return *m_typeSystem;
}
