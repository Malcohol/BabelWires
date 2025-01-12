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
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

babelwires::AddNodeForInputTreeValueCommand::AddNodeForInputTreeValueCommand(std::string commandName,
                                                                             NodeId originalNodeId, Path pathToValue,
                                                                             UiPosition positionForNewNode,
                                                                             RelationshipToOldNode relationship)
    : Command(commandName)
    , m_originalNodeId(originalNodeId)
    , m_pathToValue(pathToValue)
    , m_positionForNewNode(positionForNewNode)
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

    if (!m_pathToValue.tryFollow(*nodeInput)) {
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
    const ValueTreeNode& originalValue = m_pathToValue.follow(*nodeInput);

    ValueNodeData newNodeData(originalValue.getTypeRef());
    newNodeData.m_id = m_newNodeId;
    newNodeData.m_uiData.m_uiPosition = m_positionForNewNode;

    for (auto modifier : originalNode->getEdits().modifierRange(m_pathToValue)) {
        std::unique_ptr<ModifierData> modifierData = modifier->getModifierData().clone();
        Path& newModifierPath = modifierData->m_targetPath;
        assert(m_pathToValue.isPrefixOf(newModifierPath));
        newModifierPath.removePrefix(m_pathToValue.getNumSteps());
        newNodeData.m_modifiers.emplace_back(std::move(modifierData));
    }
    // TODO tree expansion.

    project.addNode(newNodeData);

    if (m_relationship == RelationshipToOldNode::Source) {
        //TODO Remove old modifiers. Add a connection.
    }
}

void babelwires::AddNodeForInputTreeValueCommand::undo(Project& project) const {
    if (m_relationship == RelationshipToOldNode::Source) {
        // TODO
    }

    project.removeNode(m_newNodeId);
}

babelwires::NodeId babelwires::AddNodeForInputTreeValueCommand::getNodeId() const {
    return m_newNodeId;
}
