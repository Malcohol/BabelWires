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
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>

babelwires::AddNodeForOutputTreeValueCommand::AddNodeForOutputTreeValueCommand(std::string commandName,
                                                                               NodeId originalNodeId, Path pathToValue,
                                                                               UiPosition positionForNewNode, RelationshipToDependentNodes relationship)
    : AddNodeForTreeValueCommandBase(commandName, originalNodeId, std::move(pathToValue), positionForNewNode)
    , m_relationship(relationship) {}

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

namespace {
    using ConnectionTupleCopy = std::tuple<babelwires::ConnectionModifierData, babelwires::NodeId>;
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

    if (m_relationship == RelationshipToDependentNodes::NewParent) {
        std::vector<ConnectionTupleCopy> connectionsToAdjust;

        const Project::ConnectionInfo& connectionInfo = project.getConnectionInfo();
        const auto it = connectionInfo.m_requiredFor.find(originalNode);
        if (it != connectionInfo.m_requiredFor.end()) {
            const Project::ConnectionInfo::Connections& connections = it->second;
            for (const auto& connectionTuple : connections) {
                const ConnectionModifierData& connectionData = std::get<0>(connectionTuple)->getModifierData();
                const NodeId nodeId = std::get<1>(connectionTuple)->getNodeId();
                if (m_pathToValue.isPrefixOf(connectionData.m_sourcePath)) {
                    connectionsToAdjust.emplace_back(ConnectionTupleCopy{connectionData, nodeId});
                }
            }
        }
        for (auto& [connectionData, nodeId] : connectionsToAdjust) {
            project.removeModifier(nodeId, connectionData.m_targetPath, false);
            connectionData.m_sourceId = m_newNodeId;
            connectionData.m_sourcePath.removePrefix(m_pathToValue.getNumSteps());
            project.addModifier(nodeId, connectionData, false);
        }
    }

    ConnectionModifierData newConnection;
    newConnection.m_sourceId = m_originalNodeId;
    newConnection.m_sourcePath = m_pathToValue;
    project.addModifier(m_newNodeId, newConnection);
}

void babelwires::AddNodeForOutputTreeValueCommand::undo(Project& project) const {
    project.removeModifier(m_newNodeId, Path(), false);
    
    if (m_relationship == RelationshipToDependentNodes::NewParent) {
        std::vector<ConnectionTupleCopy> connectionsToAdjust;

        const Node* const newNode = project.getNode(m_newNodeId);
        assert(newNode);

        const Project::ConnectionInfo& connectionInfo = project.getConnectionInfo();
        const auto it = connectionInfo.m_requiredFor.find(newNode);
        if (it != connectionInfo.m_requiredFor.end()) {
            const Project::ConnectionInfo::Connections& connections = it->second;
            for (const auto& connectionTuple : connections) {
                const ConnectionModifierData& connectionData = std::get<0>(connectionTuple)->getModifierData();
                const NodeId nodeId = std::get<1>(connectionTuple)->getNodeId();
                connectionsToAdjust.emplace_back(ConnectionTupleCopy{connectionData, nodeId});
            }
        }
        for (auto& [connectionData, nodeId] : connectionsToAdjust) {
            project.removeModifier(nodeId, connectionData.m_targetPath, false);
            connectionData.m_sourceId = m_originalNodeId;
            Path oldSourcePath = std::move(connectionData.m_sourcePath);
            connectionData.m_sourcePath = m_pathToValue;
            connectionData.m_sourcePath.append(oldSourcePath);
            project.addModifier(nodeId, connectionData, false);
        }
    }
    project.removeNode(m_newNodeId);
}
