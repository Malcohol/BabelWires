/**
 *
 * Copyright (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#pragma once

#include <BabelWiresLib/Project/project.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/ProjectExtra/projectObserver.hpp>

#include <QtNodes/AbstractGraphModel>

namespace babelwires {
    class Project;
    struct ProjectContext;
    class ConnectionModifier;
    class Node;
    class ModifyModelScope;
    class AccessModelScope;
    struct UiPosition;
    class AddNodeCommand;
    struct UiProjectContext;
    struct ProjectData;
    class MainWindow;
    class NodeNodeModel;

    class ProjectGraphModel : public AbstractGraphModel {
      public:
        ProjectGraphModel(Project& project, CommandManager<Project>& commandManager, UiProjectContext& projectContext);

      public:
        std::unordered_set<QtNodes::NodeId> allNodeIds() const override;
        std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const override;
        std::unordered_set<ConnectionId> connections(NodeId nodeId, PortType portType, PortIndex index) const override;
        bool connectionExists(ConnectionId const connectionId) const override;
        bool connectionPossible(ConnectionId const connectionId) const override;
        void addConnection(ConnectionId const connectionId) override;
        bool nodeExists(QtNodes::NodeId const nodeId) const override;
        QVariant nodeData(QtNodes::NodeId nodeId, NodeRole role) const override;
        QtNodes::NodeFlags nodeFlags(QtNodes::NodeId nodeId) const override;
        bool setNodeData(QtNodes::NodeId nodeId, NodeRole role, QVariant value) override;
        QVariant portData(QtNodes::NodeId nodeId, PortType portType, PortIndex index, PortRole role) const override;
        bool deleteConnection(ConnectionId const connectionId) override;
        bool deleteNode(QtNodes::NodeId const nodeId) override;

    public:
        /// Called from setNodeData.
        void nodeMoved(QtNodes::NodeId& n, const QPointF& newLocation);
        
        /// Called from setNodeData.
        void nodeResized(QtNodes::NodeId& n, const QSize& newSize);

      private:
        // Part of the interface, but not needed in this use-case.

        bool setPortData(QtNodes::NodeId nodeId, PortType portType, PortIndex index, QVariant const& value,
                         PortRole role = PortRole::Data) override {
            assert(false);
        };
        QtNodes::NodeId newNodeId() override { assert(false); };
        QtNodes::NodeId addNode(QString const nodeType = QString()) override { assert(false); };

      private:
        friend AccessModelScope;
        friend ModifyModelScope;

        Project& m_project;
        CommandManager<Project>& m_commandManager;
        UiProjectContext& m_projectContext;

        /// We use an explicit state variable.
        enum class State {
            /// The bridge is waiting for events from the flow scene.
            /// This is the state between user actions.
            ListeningToFlowScene,

            /// Changes to the model require the bridge to update the flow scene.
            /// We ignore flowScene callbacks while in this state.
            ProcessingModelChanges,

            /// The bridge expects the flow scene to inform it about a new node.
            WaitingForNewNode,
        };

        /// The state of the project bridge.
        State m_state;

        std::unordered_map<QtNodes::NodeId, std::unique_ptr<NodeNodeModel>> m_nodeModels;
    };

} // namespace babelwires
