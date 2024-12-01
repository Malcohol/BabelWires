/**
 * Nodes are the fundimental constituent of the project.
 * They expose input and output Features, and carry edits.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/TypeSystem/compoundType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/types.hpp>

#include <algorithm>

babelwires::Node::Node(const NodeData& data, NodeId newId)
    : m_data(data.customClone())
    , m_contentsCache(m_edits) {
    m_data->m_id = newId;

    for (const auto& m : data.m_modifiers) {
        // Don't apply now, since the input has not been constructed by the subclass.
        addModifierWithoutApplyingIt(*m);
    }

    for (const auto& p : data.m_expandedPaths) {
        m_edits.setExpanded(p, true);
    }
}

void babelwires::Node::applyLocalModifiers(UserLogger& userLogger) {
    ValueTreeNode* input = doGetInputNonConst();
    if (input) {
        input->setToDefault();
        modifyValueAt(input, Path());
    }
    for (auto* m : m_edits.modifierRange<Modifier>()) {
        // The modifiers are stored in path order, so parents will come before
        // children. Thus, there is no concern that a modifier will be applied
        // before a structural modifier (e.g. array size) it depends on.
        m->applyIfLocal(userLogger, input);
    }
}

babelwires::Node::~Node() = default;

babelwires::ValueTreeNode* babelwires::Node::doGetOutputNonConst() {
    return nullptr;
}

const babelwires::ValueTreeNode* babelwires::Node::getOutput() const {
    return nullptr;
}

babelwires::ValueTreeNode* babelwires::Node::getInputNonConst(const Path& pathToModify) {
    if (ValueTreeNode* input = doGetInputNonConst()) {
        modifyValueAt(input, pathToModify);
        return input;
    }
    return nullptr;
}

babelwires::ValueTreeNode* babelwires::Node::doGetInputNonConst() {
    return nullptr;
}

const babelwires::ValueTreeNode* babelwires::Node::getInput() const {
    return nullptr;
}

babelwires::NodeId babelwires::Node::getNodeId() const {
    return m_data->m_id;
}

const babelwires::NodeData& babelwires::Node::getElementData() const {
    return *m_data;
}

babelwires::NodeData& babelwires::Node::getElementData() {
    return *m_data;
}

std::string babelwires::Node::getLabel() const {
    return std::to_string(getNodeId()) + " - " + m_factoryName;
}

void babelwires::Node::setFactoryName(std::string factoryName) {
    m_factoryName = std::move(factoryName);
}

void babelwires::Node::setFactoryName(LongId identifier) {
    m_factoryName = IdentifierRegistry::read()->getName(identifier);
}

babelwires::Modifier* babelwires::Node::findModifier(const Path& featurePath) {
    return m_edits.findModifier(featurePath);
}

const babelwires::Modifier* babelwires::Node::findModifier(const Path& featurePath) const {
    return m_edits.findModifier(featurePath);
}

babelwires::Modifier* babelwires::Node::addModifierWithoutApplyingIt(const ModifierData& modifierData) {
    assert(!findModifier(modifierData.m_targetPath) && "There's already a modifier at that path");
    auto modifier = modifierData.createModifier();
    Modifier* rawModifierPtr = modifier.get();
    modifier->setOwner(this);
    m_edits.addModifier(std::move(modifier));
    setChanged(Changes::ModifierAdded);
    return rawModifierPtr;
}

babelwires::Modifier* babelwires::Node::addModifier(UserLogger& userLogger,
                                                              const ModifierData& modifierData) {
    Modifier* newModifier = addModifierWithoutApplyingIt(modifierData);
    newModifier->applyIfLocal(userLogger, getInputNonConst(newModifier->getTargetPath()));
    return newModifier;
}

void babelwires::Node::removeModifier(Modifier* modifier) {
    assert((const_cast<const Modifier*>(modifier)->getOwner() == this) &&
           "This Node is not the owner of the modifier");

    m_removedModifiers.emplace_back(std::move(m_edits.removeModifier(modifier)));
    ValueTreeNode* input = getInputNonConst(modifier->getTargetPath());
    assert(input && "Modifiable elements always have input features");
    if (!modifier->isFailed()) {
        modifier->unapply(input);
    }
    modifier->setOwner(nullptr);
    setChanged(Changes::ModifierRemoved);
}

std::unique_ptr<babelwires::NodeData> babelwires::Node::extractElementData() const {
    auto data = getElementData().clone();
    for (const auto& m : m_edits.modifierRange()) {
        data->m_modifiers.emplace_back(m->getModifierData().clone());
    }
    data->m_expandedPaths = m_edits.getAllExplicitlyExpandedPaths();
    // Strip out the currently unused paths.
    auto it = std::remove_if(data->m_expandedPaths.begin(), data->m_expandedPaths.end(), [this](const Path& p) {
        if (const ValueTreeNode* input = getInput()) {
            if (p.tryFollow(*input)) {
                return false;
            }
        }
        if (const ValueTreeNode* output = getOutput()) {
            if (p.tryFollow(*output)) {
                return false;
            }
        }
        return true;
    });
    data->m_expandedPaths.erase(it, data->m_expandedPaths.end());
    return data;
}

const babelwires::UiPosition& babelwires::Node::getUiPosition() const {
    return m_data->m_uiData.m_uiPosition;
}

void babelwires::Node::setUiPosition(const UiPosition& newPosition) {
    m_data->m_uiData.m_uiPosition = newPosition;
    setChanged(Changes::UiPositionChanged);
}

const babelwires::UiSize& babelwires::Node::getUiSize() const {
    return m_data->m_uiData.m_uiSize;
}

void babelwires::Node::setUiSize(const UiSize& newSize) {
    m_data->m_uiData.m_uiSize = newSize;
    setChanged(Changes::UiSizeChanged);
}

bool babelwires::Node::isChanged(Changes changes) const {
    if ((m_changes & changes) != Changes::NothingChanged) {
        return true;
    }
    // TODO Static assert
    const ValueTreeNode::Changes featureChanges = static_cast<ValueTreeNode::Changes>(
        static_cast<unsigned int>(changes) & static_cast<unsigned int>(Changes::FeatureChangesMask));
    if (featureChanges != ValueTreeNode::Changes::NothingChanged) {
        if (const ValueTreeNode* f = getInput()) {
            if (f->isChanged(featureChanges)) {
                return true;
            }
        }
        if (const ValueTreeNode* f = getOutput()) {
            if (f->isChanged(featureChanges)) {
                return true;
            }
        }
    }
    return false;
}

void babelwires::Node::clearChanges() {
    if (ValueTreeNode* f = doGetInputNonConst()) {
        f->clearChanges();
    }
    if (ValueTreeNode* f = doGetOutputNonConst()) {
        f->clearChanges();
    }
    if (isChanged(Changes::ModifierChangesMask | Changes::CompoundExpandedOrCollapsed | Changes::NodeIsNew)) {
        m_edits.clearChanges();
        m_removedModifiers.clear();
    }
    m_changes = Changes::NothingChanged;
    m_contentsCache.clearChanges();
}

void babelwires::Node::setChanged(Changes changes) {
    m_changes = m_changes | changes;
}

bool babelwires::Node::isFailed() const {
    return !m_internalFailure.empty() || m_isInDependencyLoop;
}

std::string babelwires::Node::getReasonForFailure() const {
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

void babelwires::Node::clearInternalFailure() {
    if (!m_internalFailure.empty()) {
        if (!m_isInDependencyLoop) {
            setChanged(Changes::NodeRecovered);
        }
        m_internalFailure.clear();
    }
}

void babelwires::Node::setInternalFailure(std::string reasonForFailure) {
    assert(!reasonForFailure.empty() && "A reason must be provided");
    if (!isFailed()) {
        setChanged(Changes::NodeFailed);
    }
    m_internalFailure = std::move(reasonForFailure);
}

bool babelwires::Node::isInDependencyLoop() const {
    return m_isInDependencyLoop;
}

void babelwires::Node::setInDependencyLoop(bool isInLoop) {
    if (isInLoop) {
        if (!isFailed()) {
            setChanged(Changes::NodeFailed);
        }
    } else {
        if (m_isInDependencyLoop && m_internalFailure.empty()) {
            setChanged(Changes::NodeRecovered);
        }
    }
    m_isInDependencyLoop = isInLoop;
}

const babelwires::ContentsCache& babelwires::Node::getContentsCache() const {
    return m_contentsCache;
}

void babelwires::Node::process(Project& project, UserLogger& userLogger) {
    finishModifications(project, userLogger);
    doProcess(userLogger);
}

void babelwires::Node::adjustArrayIndices(const babelwires::Path& pathToArray,
                                                    babelwires::ArrayIndex startIndex, int adjustment) {
    m_edits.adjustArrayIndices(pathToArray, startIndex, adjustment);
}

bool babelwires::Node::isExpanded(const babelwires::Path& featurePath) const {
    return m_edits.isExpanded(featurePath);
}

void babelwires::Node::setExpanded(const babelwires::Path& featurePath, bool expanded) {
    m_edits.setExpanded(featurePath, expanded);
    setChanged(Changes::CompoundExpandedOrCollapsed);
}

void babelwires::Node::setModifierMoving(const babelwires::Modifier& modifierAboutToMove) {
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
        for (auto* const subFeature : babelwires::getChildRange(compound)) {
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

void babelwires::Node::modifyValueAt(ValueTreeNode* input, const Path& path) {
    assert((input != nullptr) && "Trying to modify a feature element with no input feature");

    unsigned int j = 0;
    for (unsigned int i = 0; i < m_modifiedPaths.size(); ++i) {
        const auto& p = m_modifiedPaths[i];
        if (p.isPrefixOf(path)) {
            assert(i == j);
            return;
        }
        if (path.isPrefixOf(p)) {
            // Remove the path at i by not incrementing j in this iteration.
        } else {
            // Keep this path.
            if (j < i) {
                m_modifiedPaths[j] = m_modifiedPaths[i];
            }
            ++j;
        }
    }
    m_modifiedPaths.resize(j);
    m_modifiedPaths.emplace_back(path);
}

void babelwires::Node::finishModifications(const Project& project, UserLogger& userLogger) {
    // Reapply modifiers beneath the modified paths.
    for (const auto& p : m_modifiedPaths) {
        // Get the input feature directly.
        ValueTreeNode* input = doGetInputNonConst();
        // First, apply any other modifiers which apply beneath the path
        for (auto it : m_edits.modifierRange(p)) {
            if (const auto& connection = it->as<ConnectionModifier>()) {
                // We force connections in this case.
                connection->applyConnection(project, userLogger, input, true);
            } else {
                it->applyIfLocal(userLogger, input);
            }
        }
    }
    m_modifiedPaths.clear();
}


void babelwires::Node::setValueTrees(std::string rootLabel, ValueTreeRoot* input,
                                               const ValueTreeRoot* output) {
    m_contentsCache.setValueTrees(std::move(rootLabel), input, output);
}

void babelwires::Node::updateModifierCache() {
    m_contentsCache.updateModifierCache();
}
