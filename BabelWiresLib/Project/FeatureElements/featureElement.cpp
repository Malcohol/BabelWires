/**
 * FeatureElements are the fundimental constituent of the project.
 * They expose input and output Features, and carry edits.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/modifyFeatureScope.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/types.hpp>

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
    ValueTreeNode* inputFeature = doGetInputFeatureNonConst();
    if (inputFeature) {
        inputFeature->setToDefault();
        modifyFeatureAt(inputFeature, Path());
    }
    for (auto* m : m_edits.modifierRange<Modifier>()) {
        // The modifiers are stored in path order, so parents will come before
        // children. Thus, there is no concern that a modifier will be applied
        // before a structural modifier (e.g. array size) it depends on.
        m->applyIfLocal(userLogger, inputFeature);
    }
}

babelwires::FeatureElement::~FeatureElement() = default;

babelwires::ValueTreeNode* babelwires::FeatureElement::doGetOutputFeatureNonConst() {
    return nullptr;
}

const babelwires::ValueTreeNode* babelwires::FeatureElement::getOutputFeature() const {
    return nullptr;
}

babelwires::ValueTreeNode* babelwires::FeatureElement::getInputFeatureNonConst(const Path& pathToModify) {
    if (ValueTreeNode* inputFeature = doGetInputFeatureNonConst()) {
        modifyFeatureAt(inputFeature, pathToModify);
        return inputFeature;
    }
    return nullptr;
}

babelwires::ValueTreeNode* babelwires::FeatureElement::doGetInputFeatureNonConst() {
    return nullptr;
}

const babelwires::ValueTreeNode* babelwires::FeatureElement::getInputFeature() const {
    return nullptr;
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

void babelwires::FeatureElement::setFactoryName(LongId identifier) {
    m_factoryName = IdentifierRegistry::read()->getName(identifier);
}

babelwires::Modifier* babelwires::FeatureElement::findModifier(const Path& featurePath) {
    return m_edits.findModifier(featurePath);
}

const babelwires::Modifier* babelwires::FeatureElement::findModifier(const Path& featurePath) const {
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
    newModifier->applyIfLocal(userLogger, getInputFeatureNonConst(newModifier->getPathToFeature()));
    return newModifier;
}

void babelwires::FeatureElement::removeModifier(Modifier* modifier) {
    assert((const_cast<const Modifier*>(modifier)->getOwner() == this) &&
           "This FeatureElement is not the owner of the modifier");

    m_removedModifiers.emplace_back(std::move(m_edits.removeModifier(modifier)));
    ValueTreeNode* inputFeature = getInputFeatureNonConst(modifier->getPathToFeature());
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
    data->m_expandedPaths = m_edits.getAllExplicitlyExpandedPaths();
    // Strip out the currently unused paths.
    auto it = std::remove_if(data->m_expandedPaths.begin(), data->m_expandedPaths.end(), [this](const Path& p) {
        if (const ValueTreeNode* inputFeature = getInputFeature()) {
            if (p.tryFollow(*inputFeature)) {
                return false;
            }
        }
        if (const ValueTreeNode* outputFeature = getOutputFeature()) {
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
    const ValueTreeNode::Changes featureChanges = static_cast<ValueTreeNode::Changes>(
        static_cast<unsigned int>(changes) & static_cast<unsigned int>(Changes::FeatureChangesMask));
    if (featureChanges != ValueTreeNode::Changes::NothingChanged) {
        if (const ValueTreeNode* f = getInputFeature()) {
            if (f->isChanged(featureChanges)) {
                return true;
            }
        }
        if (const ValueTreeNode* f = getOutputFeature()) {
            if (f->isChanged(featureChanges)) {
                return true;
            }
        }
    }
    return false;
}

void babelwires::FeatureElement::clearChanges() {
    if (ValueTreeNode* f = doGetInputFeatureNonConst()) {
        f->clearChanges();
    }
    if (ValueTreeNode* f = doGetOutputFeatureNonConst()) {
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

void babelwires::FeatureElement::process(Project& project, UserLogger& userLogger) {
    finishModifications(project, userLogger);
    doProcess(userLogger);
}

void babelwires::FeatureElement::adjustArrayIndices(const babelwires::Path& pathToArray,
                                                    babelwires::ArrayIndex startIndex, int adjustment) {
    m_edits.adjustArrayIndices(pathToArray, startIndex, adjustment);
}

bool babelwires::FeatureElement::isExpanded(const babelwires::Path& featurePath) const {
    return m_edits.isExpanded(featurePath);
}

void babelwires::FeatureElement::setExpanded(const babelwires::Path& featurePath, bool expanded) {
    m_edits.setExpanded(featurePath, expanded);
    setChanged(Changes::CompoundExpandedOrCollapsed);
}

void babelwires::FeatureElement::setModifierMoving(const babelwires::Modifier& modifierAboutToMove) {
    setChanged(Changes::ModifierMoved);
    m_removedModifiers.emplace_back(modifierAboutToMove.clone());
}

namespace {

    babelwires::ValueTreeNode* tryFollowPathToValue(babelwires::ValueTreeNode* start, const babelwires::Path& p,
                                              int index) {
        if ((index < p.getNumSteps()) && !start->as<babelwires::ValueTreeRoot>()) {
            if (auto* compound = start) {
                babelwires::ValueTreeNode& child = compound->getChildFromStep(p.getStep(index));
                return tryFollowPathToValue(&child, p, index + 1);
            } else {
                return nullptr;
            }
        } else {
            return start;
        }
    }

    babelwires::ValueTreeNode* tryFollowPathToValueSafe(babelwires::ValueTreeNode* start, const babelwires::Path& p) {
        try {
            return tryFollowPathToValue(start, p, 0);
        } catch (...) {
            return nullptr;
        }
    }

    babelwires::ValueTreeRoot* exploreForCompoundRootValueFeature(babelwires::ValueTreeNode* compound) {
        // TODO: Out of date.
        for (auto* const subFeature : babelwires::subfeatures(compound)) {
            if (auto* const simpleValueFeature = subFeature->as<babelwires::ValueTreeRoot>()) {
                if (simpleValueFeature->getType().as<babelwires::CompoundType>()) {
                    return simpleValueFeature;
                }
            } else {
                if (auto* const simpleValueFeature = exploreForCompoundRootValueFeature(subFeature)) {
                    return simpleValueFeature;
                }
            }
        }
        return nullptr;
    }

} // namespace

void babelwires::FeatureElement::modifyFeatureAt(ValueTreeNode* inputFeature, const Path& p) {
    assert((inputFeature != nullptr) && "Trying to modify a feature element with no input feature");

    // This code assumes there's only ever one compound value type in a feature tree.

    if (m_modifyFeatureScope != nullptr) {
        return;
    }

    // Look for a root value feature in the ancestor chain.
    ValueTreeNode* target = tryFollowPathToValueSafe(inputFeature, p);
    if (!target) {
        // For now, it's not the job of this method to handle failures.
        // The modifier will reattempt the traversal and capture the failure properly.
        return;
    }

    ValueTreeRoot* rootValueFeature = nullptr;

    if (ValueTreeRoot *const valueFeature = target->as<ValueTreeRoot>()) {
        // Modification below a compound root value feature.
        if (valueFeature->getType().as<CompoundType>()) {
            rootValueFeature = valueFeature;
        }
    } else {
        // If a modification is being made _above_ a compound root value feature, we also need to back it up.
        // This is necessary when deserializing nodes with modifications to sub-values, or when the deletion of
        // a node is undone.
        rootValueFeature = exploreForCompoundRootValueFeature(target);
    }

    if (rootValueFeature) {
        rootValueFeature->backUpValue();
        m_modifyFeatureScope = std::make_unique<ModifyFeatureScope>(Path(rootValueFeature), rootValueFeature);
    }
}

void babelwires::FeatureElement::finishModifications(const Project& project, UserLogger& userLogger) {
    if (m_modifyFeatureScope) {
        // Get the input feature directly.
        ValueTreeNode* inputFeature = doGetInputFeatureNonConst();
        // First, apply any other modifiers which apply beneath the path
        for (auto it : m_edits.modifierRange(m_modifyFeatureScope->m_pathToRootValue)) {
            if (const auto& connection = it->as<ConnectionModifier>()) {
                // We force connections in this case.
                connection->applyConnection(project, userLogger, inputFeature, true);
            } else {
                it->applyIfLocal(userLogger, inputFeature);
            }
        }

        // The SimpleValueFeature can now determine its change flags.
        m_modifyFeatureScope->m_rootValueFeature->reconcileChangesFromBackup();
        m_modifyFeatureScope = nullptr;
    }
}
