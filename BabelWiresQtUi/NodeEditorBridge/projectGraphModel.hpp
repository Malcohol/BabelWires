/**
 *
 * Copyright (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#pragma once

#include <BabelWiresLib/Project/project.hpp>

#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/ProjectExtra/connectionDescription.hpp>
#include <BabelWiresLib/ProjectExtra/projectObserver.hpp>

#include <Common/multiKeyMap.hpp>

#include <QtNodes/AbstractGraphModel>

#include <QGraphicsView>

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

    class ProjectGraphModel : public QtNodes::AbstractGraphModel {
        Q_OBJECT
      public:
        ProjectGraphModel(Project& project, CommandManager<Project>& commandManager, UiProjectContext& projectContext);
        ~ProjectGraphModel();

        /// The command will be executed when Qt is idle.
        /// Only one command can be scheduled.
        void scheduleCommand(std::unique_ptr<Command<Project>> command);

        /// Execute an Command now.
        /// Almost always prefer scheduleCommand: this should only be used if explicit notification is
        /// needed when the command fails.
        /// For example, when a complex editor fails to apply its contents to the project, we want to
        /// very explicitly notify the user so they do not lose lots of work.
        /// Returns true if the command succeeded.
        bool executeCommandSynchronously(std::unique_ptr<Command<Project>> command);

        /// Get a pointer to the main window, which is needed when creating editors for values such as maps.
        MainWindow* getMainWindow() const;
        
        /// This can be used as the parent of dialogs and menus.
        QGraphicsView* getFlowGraphWidget();

        void setWidgets(MainWindow* mainWindow, QGraphicsView* flowGraphWidget);

        const UiProjectContext& getContext() const;
      public:
        // The AbstractGraphModel interface.
        std::unordered_set<QtNodes::NodeId> allNodeIds() const override;
        std::unordered_set<QtNodes::ConnectionId> allConnectionIds(QtNodes::NodeId const nodeId) const override;
        std::unordered_set<QtNodes::ConnectionId> connections(QtNodes::NodeId nodeId, QtNodes::PortType portType, QtNodes::PortIndex index) const override;
        bool connectionExists(QtNodes::ConnectionId const connectionId) const override;
        bool connectionPossible(QtNodes::ConnectionId const connectionId) const override;
        void addConnection(QtNodes::ConnectionId const connectionId) override;
        bool nodeExists(QtNodes::NodeId const nodeId) const override;
        QVariant nodeData(QtNodes::NodeId nodeId, QtNodes::NodeRole role) const override;
        QtNodes::NodeFlags nodeFlags(QtNodes::NodeId nodeId) const override;
        bool setNodeData(QtNodes::NodeId nodeId, QtNodes::NodeRole role, QVariant value) override;
        QVariant portData(QtNodes::NodeId nodeId, QtNodes::PortType portType, QtNodes::PortIndex index, QtNodes::PortRole role) const override;
        bool deleteConnection(QtNodes::ConnectionId const connectionId) override;
        bool deleteNode(QtNodes::NodeId const nodeId) override;

        // Parts of the interface that are not needed in this use-case.
        bool setPortData(QtNodes::NodeId nodeId, QtNodes::PortType portType, QtNodes::PortIndex index, QVariant const& value,
          QtNodes::PortRole role = QtNodes::PortRole::Data) override {
            assert(false);
        };
        QtNodes::NodeId newNodeId() override { assert(false); };
        QtNodes::NodeId addNode(QString const nodeType = QString()) override { assert(false); };

      private:
        /// Called from setNodeData.
        void nodeMoved(QtNodes::NodeId n, const QPointF& newLocation);

        /// Called from setNodeData.
        void nodeResized(QtNodes::NodeId n, const QSize& newSize);

        QString getNodeCaption(QtNodes::NodeId nodeId) const;

      private:
        QtNodes::ConnectionId createConnectionIdFromConnectionDescription(const AccessModelScope& scope,
                                                                          const ConnectionDescription& connection);
        ConnectionDescription createConnectionDescriptionFromConnectionId(const AccessModelScope& scope,
                                                                          const QtNodes::ConnectionId& connectionId);
      private:
        // Changes caused by the model

        /// Add the described connection to the flow scene.
        void addConnectionToFlowScene(const ConnectionDescription& connection);

        /// Remove the described connection from the flow scene.
        void removeConnectionFromFlowScene(const ConnectionDescription& connection);

        /// Add the node but not the connection to the flow scene.
        void addNodeToFlowScene(const Node* node);

        /// Remove the node from the flow scene.
        void removeNodeFromFlowScene(NodeId elementId);

      private:
        /// Respond to changes in the model.
        void processAndHandleModelChanges();

        // Callback that can be fired when Qt is idle.
        void onIdle();

      private:
        friend AccessModelScope;
        friend ModifyModelScope;

        Project& m_project;
        CommandManager<Project>& m_commandManager;
        UiProjectContext& m_projectContext;

        MainWindow* m_mainWindow = nullptr;
        QGraphicsView* m_flowGraphWidget = nullptr;

        /// We use an explicit state variable.
        enum class State {
            /// The bridge is waiting for events from the flow scene.
            /// This is the state between user actions.
            ListeningToFlowScene,

            /// Changes to the model require the bridge to update the flow scene.
            /// We ignore non-const flowScene callbacks while in this state.
            ProcessingModelChanges
        };

        class StateScope;
        friend class StateScope;

        /// The state of the project bridge.
        State m_state;

        /// Observe changes in the project, and fire signals.
        ProjectObserver m_projectObserver;

        /// Subscribe to the project observer's signals.
        std::vector<SignalSubscription> m_projectObserverSubscriptions;

        std::unordered_map<QtNodes::NodeId, std::unique_ptr<NodeNodeModel>> m_nodeModels;

        /// Most commands are scheduled to run when the UI is idle, rather than performed synchronously.
        /// One reason is that the processing of some commands modify the UI, causing inconsistencies
        /// and crashes.
        std::unique_ptr<Command<Project>> m_scheduledCommand;

        bool m_newNodesShouldBeSelected = false;
    };

} // namespace babelwires
