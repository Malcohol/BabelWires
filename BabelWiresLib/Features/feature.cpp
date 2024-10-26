/**
 * A Feature is a self-describing data-structure which stores the data in the model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/feature.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

babelwires::Feature::~Feature() {}

void babelwires::Feature::setOwner(Feature* owner) {
    m_owner = owner;
}

const babelwires::Feature* babelwires::Feature::getOwner() const {
    return m_owner;
}

babelwires::Feature* babelwires::Feature::getOwnerNonConst() {
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
    for (auto&& child : subfeatures(*this)) {
        child->clearChanges();
    }
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

namespace {
    void checkIndex(const babelwires::Feature* f, int i) {
        if ((i < 0) || (i >= f->getNumFeatures())) {
            throw babelwires::ModelException()
                << "Compound feature with " << f->getNumFeatures() << " children queried by index " << i;
        }
    }

} // namespace

babelwires::Feature* babelwires::Feature::getFeature(int i) {
    checkIndex(this, i);
    return doGetFeature(i);
}

const babelwires::Feature* babelwires::Feature::getFeature(int i) const {
    checkIndex(this, i);
    return doGetFeature(i);
}

void babelwires::Feature::setSubfeaturesToDefault() {
    for (auto&& child : subfeatures(*this)) {
        child->setToDefault();
    }
}

namespace {

    template <typename COMPOUND>
    typename babelwires::CopyConst<COMPOUND, babelwires::Feature>::type*
    tryGetChildFromStepT(COMPOUND* compound, const babelwires::PathStep& step) {
        const int childIndex = compound->getChildIndexFromStep(step);
        if (childIndex >= 0) {
            return compound->getFeature(childIndex);
        }
        return nullptr;
    }

} // namespace

babelwires::Feature* babelwires::Feature::tryGetChildFromStep(const PathStep& step) {
    return tryGetChildFromStepT(this, step);
}

const babelwires::Feature* babelwires::Feature::tryGetChildFromStep(const PathStep& step) const {
    return tryGetChildFromStepT(this, step);
}

babelwires::Feature& babelwires::Feature::getChildFromStep(const PathStep& step) {
    if (Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}

const babelwires::Feature& babelwires::Feature::getChildFromStep(const PathStep& step) const {
    if (const Feature* f = tryGetChildFromStep(step)) {
        return *f;
    } else {
        throw babelwires::ModelException() << "Compound has no child at step \"" << step << "\"";
    }
}
