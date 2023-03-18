/**
 * A Feature is a self-describing data-structure which stores the data in the model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/feature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/compoundFeature.hpp>

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

babelwires::Feature::Style babelwires::Feature::getStyle() const {
    return Style::isCollapsable;
}
