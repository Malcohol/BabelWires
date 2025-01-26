/**
 * The command which adds Nodes to the project.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Project/Commands/addNodeForOutputTreeValueCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

babelwires::AddNodeForOutputTreeValueCommand::AddNodeForOutputTreeValueCommand(std::string commandName,
                                                                               NodeId originalNodeId, Path pathToValue,
                                                                               UiPosition positionForNewNode)
    : Command(commandName)
    , m_originalNodeId(originalNodeId)
    , m_pathToValue(pathToValue)
    , m_positionForNewNode(positionForNewNode) {}

bool babelwires::AddNodeForOutputTreeValueCommand::initializeAndExecute(Project& project) {
    const Node* const originalNode = project.getNode(m_originalNodeId);

    if (!originalNode) {
        return false;
    }

    const ValueTreeNode* const nodeOutput = originalNode->getOutput();
    if (!nodeOutput) {
        return false;
    }

    if (!m_pathToValue.tryFollow(*nodeOutput)) {
        return false;
    }

    m_newNodeId = project.reserveNodeId();

    execute(project);

    return true;
}

void babelwires::AddNodeForOutputTreeValueCommand::execute(Project& project) const {
    const Node* const originalNode = project.getNode(m_originalNodeId);
    assert(originalNode);
    const ValueTreeNode* const nodeOutput = originalNode->getOutput();
    assert(nodeOutput);
    const ValueTreeNode& originalValue = m_pathToValue.follow(*nodeOutput);

    ValueNodeData newNodeData(originalValue.getTypeRef());
    newNodeData.m_id = m_newNodeId;
    newNodeData.m_uiData.m_uiPosition = m_positionForNewNode;

    for (auto path : originalNode->getEdits().getAllExplicitlyExpandedPaths(m_pathToValue)) {
        Path newExpandedPath = path;
        assert(m_pathToValue.isPrefixOf(newExpandedPath));
        newExpandedPath.removePrefix(m_pathToValue.getNumSteps());
        newNodeData.m_expandedPaths.emplace_back(std::move(newExpandedPath));
    }
    project.addNode(newNodeData);

    ConnectionModifierData newConnection;
    newConnection.m_sourceId = m_originalNodeId;
    newConnection.m_sourcePath = m_pathToValue;
    project.addModifier(m_newNodeId, newConnection);
}

void babelwires::AddNodeForOutputTreeValueCommand::undo(Project& project) const {
    project.removeModifier(m_newNodeId, Path(), false);
    project.removeNode(m_newNodeId);
}

babelwires::NodeId babelwires::AddNodeForOutputTreeValueCommand::getNodeId() const {
    return m_newNodeId;
}
