/**
 * A SimpleValueFeature is a ValueFeature which owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/simpleValueFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/valuePath.hpp>

babelwires::SimpleValueFeature::SimpleValueFeature(TypeRef typeRef)
    : ValueFeature(std::move(typeRef)) {}

const babelwires::ValueHolder& babelwires::SimpleValueFeature::doGetValue() const {
    // Not sure if this assert is necessary.
    assert(m_value && "The SimpleValueFeature has not been initialized");
    return m_value;
}

void babelwires::SimpleValueFeature::doSetToDefault() {
    assert(getTypeRef() && "The type must be set to something non-trivial before doSetToDefault is called");
    const ProjectContext& context = RootFeature::getProjectContextAt(*this);
    auto [newValue, _] = getType().createValue(context.m_typeSystem);
    if (m_value != newValue) {
        m_value.swap(newValue);
        synchronizeSubfeatures();
        if (newValue) {
            reconcileChanges(newValue);
        } else {
            setChanged(Changes::StructureChanged);
        }
    }
}

void babelwires::SimpleValueFeature::backUpValue() {
    assert(!m_valueBackUp && "The value is already backed-up");
    m_valueBackUp = m_value;
}

babelwires::ValueHolder& babelwires::SimpleValueFeature::setModifiable(const FeaturePath& pathFromHere) {
    if (pathFromHere.getNumSteps() > 0) {
        assert(getType().as<CompoundType>() && "Path leading into a non-compound type");
        assert(m_valueBackUp && "You cannot make a feature modifiable if its RootValueFeature has not been backed up");
        const ProjectContext& context = RootFeature::getProjectContextAt(*this);
        auto [_, valueInCopy] = followNonConst(context.m_typeSystem, getType(), pathFromHere, m_value);
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
}
