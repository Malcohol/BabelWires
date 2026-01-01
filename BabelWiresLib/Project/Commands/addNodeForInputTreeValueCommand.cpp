/**
 * The command which adds Nodes to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addNodeForInputTreeValueCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

babelwires::AddNodeForInputTreeValueCommand::AddNodeForInputTreeValueCommand(std::string commandName,
                                                                             NodeId originalNodeId, Path pathToValue,
                                                                             UiPosition positionForNewNode,
                                                                             RelationshipToOldNode relationship)
    : AddNodeForTreeValueCommandBase(commandName, originalNodeId, std::move(pathToValue), positionForNewNode)
    , m_relationship(relationship) {}

bool babelwires::AddNodeForInputTreeValueCommand::initializeAndExecute(Project& project) {
    const Node* const originalNode = project.getNode(m_originalNodeId);

    if (!originalNode) {
        return false;
    }

    const ValueTreeNode* const nodeInput = originalNode->getInput();
    if (!nodeInput) {
        return false;
    }

    if (!tryFollowPath(m_pathToValue, *nodeInput)) {
        return false;
    }

    m_newNodeId = project.reserveNodeId();

    execute(project);

    return true;
}

void babelwires::AddNodeForInputTreeValueCommand::execute(Project& project) const {
    const Node* const originalNode = project.getNode(m_originalNodeId);
    assert(originalNode);
    const ValueTreeNode* const nodeInput = originalNode->getInput();
    assert(nodeInput);
    const ValueTreeNode& originalValue = followPath(m_pathToValue, *nodeInput);

    ValueNodeData newNodeData(originalValue.getTypeExp());
    newNodeData.m_id = m_newNodeId;
    newNodeData.m_uiData.m_uiPosition = m_positionForNewNode;

    for (auto modifier : originalNode->getEdits().modifierRange(m_pathToValue)) {
        std::unique_ptr<ModifierData> modifierData = modifier->getModifierData().clone();
        Path& newModifierPath = modifierData->m_targetPath;
        assert(m_pathToValue.isPrefixOf(newModifierPath));
        newModifierPath.removePrefix(m_pathToValue.getNumSteps());
        newNodeData.m_modifiers.emplace_back(std::move(modifierData));
    }
    for (auto path : originalNode->getEdits().getAllExplicitlyExpandedPaths(m_pathToValue)) {
        Path newExpandedPath = path;
        assert(m_pathToValue.isPrefixOf(newExpandedPath));
        newExpandedPath.removePrefix(m_pathToValue.getNumSteps());
        newNodeData.m_expandedPaths.emplace_back(std::move(newExpandedPath));
    }
    project.addNode(newNodeData);

    if (m_relationship == RelationshipToOldNode::Source) {
        std::vector<Path> pathsToRemove;
        for (auto modifier : originalNode->getEdits().modifierRange(m_pathToValue)) {
            pathsToRemove.emplace_back(modifier->getTargetPath());
        }
        for (auto path : pathsToRemove) {
            // Don't unapply the modifier, since values are unaffected by this operation.
            // This is more than an optimization: Unapplying can cause a structural change to the node,
            // which is a problem when the UI is in the middle of a drag operation.
            // Also note: Since we're not unapplying the modifiers, we can remove them in their
            // tree order. (Normally, you have to remove them in reverse order.)
            project.removeModifier(m_originalNodeId, path, false);
        }
        ConnectionModifierData newConnection;
        newConnection.m_sourceId = m_newNodeId;
        newConnection.m_targetPath = m_pathToValue;
        project.addModifier(m_originalNodeId, newConnection);
    }
}

void babelwires::AddNodeForInputTreeValueCommand::undo(Project& project) const {
    if (m_relationship == RelationshipToOldNode::Source) {
        Node* const originalNode = project.getNode(m_originalNodeId);
        assert(originalNode);
        // First, remove the connection that got added.
        project.removeModifier(m_originalNodeId, m_pathToValue, false);
        // Then, copy the modifiers back to the original node.
        const Node* const addedNode = project.getNode(m_newNodeId);
        assert(addedNode);
        for (auto modifier : addedNode->getEdits().modifierRange()) {
            std::unique_ptr<ModifierData> modifierData = modifier->getModifierData().clone();
            Path newModifierPath = m_pathToValue;
            newModifierPath.append(modifierData->m_targetPath);
            modifierData->m_targetPath = newModifierPath;
            project.addModifier(m_originalNodeId, *modifierData, false);
        }
    }

    project.removeNode(m_newNodeId);
}
