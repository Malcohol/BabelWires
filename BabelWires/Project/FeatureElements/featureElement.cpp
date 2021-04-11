/**
 * FeatureElements are the fundimental constituent of the project.
 * They expose input and output Features, and carry edits.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Features/Utilities/modelUtilities.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Project/Modifiers/connectionModifier.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/project.hpp"

#include "Common/types.hpp"

#include <algorithm>

babelwires::FeatureElement::FeatureElement(const ElementData& data, ElementId newId)
    : m_data(data.customClone())
    , m_contentsCache(m_edits) {
    m_data->m_id = newId;

    for (const auto& m : data.m_modifiers) {
        // Don't apply now, since the input feature has not been constructed by the subclass.
        addModifierWithoutApplyingIt(*m);
    }

    for (const auto& p : data.m_expandedPaths) {
        m_edits.setExpanded(p, true);
    }
}

void babelwires::FeatureElement::applyLocalModifiers(UserLogger& userLogger) {
    Feature* inputFeature = getInputFeature();
    if (inputFeature) {
        inputFeature->setToDefault();
    }
    for (auto* m : m_edits.modifierRange<Modifier>()) {
        // The modifiers are stored in path order, so parents will come before
        // children. Thus, there is no concern that a modifier will be applied
        // before a structural modifier (e.g. array size) it depends on.
        m->applyIfLocal(userLogger, inputFeature);
    }
}

babelwires::FeatureElement::~FeatureElement() = default;

babelwires::RecordFeature* babelwires::FeatureElement::getOutputFeature() {
    return nullptr;
}

const babelwires::RecordFeature* babelwires::FeatureElement::getOutputFeature() const {
    return const_cast<babelwires::FeatureElement*>(this)->getOutputFeature();
}

babelwires::RecordFeature* babelwires::FeatureElement::getInputFeature() {
    return nullptr;
}

const babelwires::RecordFeature* babelwires::FeatureElement::getInputFeature() const {
    return const_cast<babelwires::FeatureElement*>(this)->getInputFeature();
}

babelwires::ElementId babelwires::FeatureElement::getElementId() const {
    return m_data->m_id;
}

const babelwires::ElementData& babelwires::FeatureElement::getElementData() const {
    return *m_data;
}

babelwires::ElementData& babelwires::FeatureElement::getElementData() {
    return *m_data;
}

std::string babelwires::FeatureElement::getLabel() const {
    return std::to_string(getElementId()) + " - " + m_factoryName;
}

void babelwires::FeatureElement::setFactoryName(std::string factoryName) {
    m_factoryName = std::move(factoryName);
}

babelwires::Modifier* babelwires::FeatureElement::findModifier(const FeaturePath& featurePath) {
    return m_edits.findModifier(featurePath);
}

const babelwires::Modifier* babelwires::FeatureElement::findModifier(const FeaturePath& featurePath) const {
    return m_edits.findModifier(featurePath);
}

babelwires::Modifier* babelwires::FeatureElement::addModifierWithoutApplyingIt(const ModifierData& modifierData) {
    assert(!findModifier(modifierData.m_pathToFeature) && "There's already a modifier at that path");
    auto modifier = modifierData.createModifier();
    Modifier* rawModifierPtr = modifier.get();
    modifier->setOwner(this);
    m_edits.addModifier(std::move(modifier));
    setChanged(Changes::ModifierAdded);
    return rawModifierPtr;
}

babelwires::Modifier* babelwires::FeatureElement::addModifier(UserLogger& userLogger,
                                                              const ModifierData& modifierData) {
    Modifier* newModifier = addModifierWithoutApplyingIt(modifierData);
    newModifier->applyIfLocal(userLogger, getInputFeature());
    return newModifier;
}

void babelwires::FeatureElement::removeModifier(Modifier* modifier) {
    assert((const_cast<const Modifier*>(modifier)->getOwner() == this) &&
           "This FeatureElement is not the owner of the modifier");

    m_removedModifiers.emplace_back(std::move(m_edits.removeModifier(modifier)));
    Feature* inputFeature = getInputFeature();
    assert(inputFeature && "Modifiable elements always have input features");
    if (!modifier->isFailed()) {
        modifier->unapply(inputFeature);
    }
    modifier->setOwner(nullptr);
    setChanged(Changes::ModifierRemoved);
}

std::unique_ptr<babelwires::ElementData> babelwires::FeatureElement::extractElementData() const {
    auto data = getElementData().clone();
    for (const auto& m : m_edits.modifierRange()) {
        data->m_modifiers.emplace_back(m->getModifierData().clone());
    }
    data->m_expandedPaths = m_edits.getAllExpandedPaths();
    // Strip out the currently unused paths.
    auto it = std::remove_if(data->m_expandedPaths.begin(), data->m_expandedPaths.end(), [this](const FeaturePath& p) {
        if (const Feature* inputFeature = getInputFeature()) {
            if (p.tryFollow(*inputFeature)) {
                return false;
            }
        }
        if (const Feature* outputFeature = getOutputFeature()) {
            if (p.tryFollow(*outputFeature)) {
                return false;
            }
        }
        return true;
    });
    data->m_expandedPaths.erase(it, data->m_expandedPaths.end());
    return data;
}

const babelwires::UiPosition& babelwires::FeatureElement::getUiPosition() const {
    return m_data->m_uiData.m_uiPosition;
}

void babelwires::FeatureElement::setUiPosition(const UiPosition& newPosition) {
    m_data->m_uiData.m_uiPosition = newPosition;
    setChanged(Changes::UiPositionChanged);
}

const babelwires::UiSize& babelwires::FeatureElement::getUiSize() const {
    return m_data->m_uiData.m_uiSize;
}

void babelwires::FeatureElement::setUiSize(const UiSize& newSize) {
    m_data->m_uiData.m_uiSize = newSize;
    setChanged(Changes::UiSizeChanged);
}

bool babelwires::FeatureElement::isChanged(Changes changes) const {
    if ((m_changes & changes) != Changes::NothingChanged) {
        return true;
    }
    // TODO Static assert
    const Feature::Changes featureChanges = static_cast<Feature::Changes>(
        static_cast<unsigned int>(changes) & static_cast<unsigned int>(Changes::FeatureChangesMask));
    if (featureChanges != Feature::Changes::NothingChanged) {
        if (const Feature* f = getInputFeature()) {
            if (f->isChanged(featureChanges)) {
                return true;
            }
        }
        if (const Feature* f = getOutputFeature()) {
            if (f->isChanged(featureChanges)) {
                return true;
            }
        }
    }
    return false;
}

void babelwires::FeatureElement::clearChanges() {
    if (Feature* f = getInputFeature()) {
        f->clearChanges();
    }
    if (Feature* f = getOutputFeature()) {
        f->clearChanges();
    }
    if (isChanged(Changes::ModifierChangesMask | Changes::CompoundExpandedOrCollapsed | Changes::FeatureElementIsNew)) {
        m_edits.clearChanges();
        m_removedModifiers.clear();
    }
    m_changes = Changes::NothingChanged;
    m_contentsCache.clearChanges();
}

void babelwires::FeatureElement::setChanged(Changes changes) {
    m_changes = m_changes | changes;
}

bool babelwires::FeatureElement::isFailed() const {
    return !m_internalFailure.empty() || m_isInDependencyLoop;
}

std::string babelwires::FeatureElement::getReasonForFailure() const {
    std::string reason;
    if (!m_internalFailure.empty()) {
        if (m_isInDependencyLoop) {
            reason = m_internalFailure + " and the element is part of a dependency loop";
        } else {
            reason = m_internalFailure;
        }
    } else if (m_isInDependencyLoop) {
        reason = "The element is part of a dependency loop";
    }
    return reason;
}

void babelwires::FeatureElement::clearInternalFailure() {
    if (!m_internalFailure.empty()) {
        if (!m_isInDependencyLoop) {
            setChanged(Changes::FeatureElementRecovered);
        }
        m_internalFailure.clear();
    }
}

void babelwires::FeatureElement::setInternalFailure(std::string reasonForFailure) {
    assert(!reasonForFailure.empty() && "A reason must be provided");
    if (!isFailed()) {
        setChanged(Changes::FeatureElementFailed);
    }
    m_internalFailure = std::move(reasonForFailure);
}

bool babelwires::FeatureElement::isInDependencyLoop() const {
    return m_isInDependencyLoop;
}

void babelwires::FeatureElement::setInDependencyLoop(bool isInLoop) {
    if (isInLoop) {
        if (!isFailed()) {
            setChanged(Changes::FeatureElementFailed);
        }
    } else {
        if (m_isInDependencyLoop && m_internalFailure.empty()) {
            setChanged(Changes::FeatureElementRecovered);
        }
    }
    m_isInDependencyLoop = isInLoop;
}

const babelwires::ContentsCache& babelwires::FeatureElement::getContentsCache() const {
    return m_contentsCache;
}

void babelwires::FeatureElement::process(UserLogger& userLogger) {
    doProcess(userLogger);
}

void babelwires::FeatureElement::adjustArrayIndices(const babelwires::FeaturePath& pathToArray,
                                                    babelwires::ArrayIndex startIndex, int adjustment) {
    m_edits.adjustArrayIndices(pathToArray, startIndex, adjustment);
}

bool babelwires::FeatureElement::isExpanded(const babelwires::FeaturePath& featurePath) const {
    return m_edits.isExpanded(featurePath);
}

void babelwires::FeatureElement::setExpanded(const babelwires::FeaturePath& featurePath, bool expanded) {
    m_edits.setExpanded(featurePath, expanded);
    setChanged(Changes::CompoundExpandedOrCollapsed);
}

void babelwires::FeatureElement::setModifierMoving(const babelwires::Modifier& modifierAboutToMove) {
    setChanged(Changes::ModifierMoved);
    m_removedModifiers.emplace_back(modifierAboutToMove.clone());
}
