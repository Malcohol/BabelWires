/**
 * A Feature is a self-describing data-structure which stores the data in the model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Features/features.hpp"

#include "BabelWiresLib/Features/Utilities/modelUtilities.hpp"
#include "BabelWiresLib/Features/modelExceptions.hpp"
#include "BabelWiresLib/Features/rootFeature.hpp"

babelwires::Feature::~Feature() {}

void babelwires::Feature::setOwner(CompoundFeature* owner) {
    m_owner = owner;
}

const babelwires::CompoundFeature* babelwires::Feature::getOwner() const {
    return m_owner;
}

void babelwires::Feature::setChanged(Changes changes) {
    assert((changes != Changes::NothingChanged) && "You cannot call setChanged with no changes to set");
    assert(((changes & ~Changes::SomethingChanged) == Changes::NothingChanged) && "Not a supported change");
    if ((m_changes | changes) != m_changes) {
        m_changes = m_changes | changes;
        if (m_owner) {
            m_owner->setChanged(changes);
        }
    }
}

bool babelwires::Feature::isChanged(Changes changes) const {
    return (m_changes & changes) != Changes::NothingChanged;
}

void babelwires::Feature::doClearChanges() {
    m_changes = Changes::NothingChanged;
}

void babelwires::Feature::clearChanges() {
    doClearChanges();
    // I could do this here, but it makes the interface less intuitive.
    assert((m_changes == Changes::NothingChanged) &&
           "doClearChanges did not make a super-call to Feature::doClearChanges");
}

void babelwires::Feature::setToDefault() {
    doSetToDefault();
}

void babelwires::Feature::setToDefaultNonRecursive() {
    doSetToDefaultNonRecursive();
}

std::size_t babelwires::Feature::getHash() const {
    return doGetHash();
}

std::string babelwires::ValueFeature::getValueType() const {
    // Future proofing, while I consider this approach.
    assert((doGetValueType().size() <= 4) && "The value type must be 4 characters or shorter");
    return doGetValueType();
}

bool babelwires::ValueFeature::isCompatible(const ValueFeature& other) {
    return getValueType() == other.getValueType();
}

void babelwires::ValueFeature::assign(const ValueFeature& other) {
    if (!isCompatible(other)) {
        throw ModelException() << "Assigning an incompatible value";
    }
    doAssign(other);
}

void babelwires::ValueFeature::doSetToDefaultNonRecursive() {
    setToDefault();
}

namespace {
    void checkIndex(const babelwires::CompoundFeature* f, int i) {
        if ((i < 0) || (i >= f->getNumFeatures())) {
            throw babelwires::ModelException()
                << "Compound feature with " << f->getNumFeatures() << " children queried by index " << i;
        }
    }

} // namespace

babelwires::Feature* babelwires::CompoundFeature::getFeature(int i) {
    checkIndex(this, i);
    return doGetFeature(i);
}

const babelwires::Feature* babelwires::CompoundFeature::getFeature(int i) const {
    checkIndex(this, i);
    return doGetFeature(i);
}

void babelwires::CompoundFeature::setSubfeaturesToDefault() {
    for (auto&& child : subfeatures(*this)) {
        child->setToDefault();
    }
}

void babelwires::CompoundFeature::doClearChanges() {
    Feature::doClearChanges();
    for (auto&& child : subfeatures(*this)) {
        child->clearChanges();
    }
}

babelwires::Feature& babelwires::CompoundFeature::getChildFromStep(const PathStep& step) {
    if (Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}

const babelwires::Feature& babelwires::CompoundFeature::getChildFromStep(const PathStep& step) const {
    if (const Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}
