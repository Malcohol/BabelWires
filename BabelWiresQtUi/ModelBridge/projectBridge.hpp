/**
 * The ProjectBridge maintains the correspondence between the FlowScene and the Project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/projectIds.hpp"
#include "BabelWiresLib/ProjectExtra/connectionDescription.hpp"
#include "BabelWiresLib/ProjectExtra/projectObserver.hpp"
#include "BabelWiresLib/Commands/commandManager.hpp"

#include "BabelWiresQtUi/ModelBridge/Internal/flowSceneConnectionInfo.hpp"

#include "Common/multiKeyMap.hpp"

#include <QObject>
#include <nodes/Node>

#include <unordered_map>
#include <unordered_set>

namespace QtNodes {
    class FlowScene;
    class Node;
    class Connection;
} // namespace QtNodes

namespace babelwires {

    class Project;
    struct ProjectContext;
    class ConnectionModifier;
    class FeatureElement;
    class ModifyModelScope;
    class AccessModelScope;
    struct UiPosition;
    class AddElementCommand;
    struct UiProjectContext;
    struct ProjectData;

    /// Class which controls the interaction between the UI and the project.
    /// To access or modify the model, use the friend classes AccessModelScope
    /// and ModifyModelScope.
    class ProjectBridge : public QObject {
        Q_OBJECT

      public:
        ProjectBridge(Project& project, CommandManager<Project>& commandManager, UiProjectContext& projectContext);
        virtual ~ProjectBridge();

        /// Connect this object's slots to the signals provided by the flow scene.
        /// The bridge will be informed when the user changes the scene, and
        /// will update the project.
        void connectToFlowScene(QtNodes::FlowScene& flowScene);

        const UiProjectContext& getContext() const;

        /// This can be used as the parent of dialogs and menus.
        QWidget* getFlowGraphWidget();

        /// The command will be executed when Qt is idle.
        /// Only one command can be scheduled.
        void scheduleCommand(std::unique_ptr<Command<Project>> command);

        /// Execute an AddElementCommand now.
        /// This special case allows the new node to be constructed knowing its corresponding model element.
        /// Returns true if the command succeeded.
        bool executeAddElementCommand(std::unique_ptr<AddElementCommand> command);

        /// Object the data from the current selection.
        ProjectData getDataFromSelectedNodes();

        /// Clears the selection, and sets a flag that any new nodes which arise
        /// when processing changes from the project should be selected.
        /// The flag is cleared after processing.
        void selectNewNodes();

      signals:
        void nodeSelectionChanged(int numNodesSelected);

      private:
        /// Find the node in the scene that corresponds to the given element id,
        /// if it exists.
        QtNodes::Node* getNodeFromId(ElementId id);

        /// Respond to changes in the model.
        void processAndHandleModelChanges();

        /// Add the described connection to the flow scene.
        void addConnectionToFlowScene(const ConnectionDescription& connection);

        /// Remove the described connection from the flow scene.
        void removeConnectionFromFlowScene(const ConnectionDescription& connection);

        /// Add the node but not the connection to the flow scene.
        void addNodeToFlowScene(const FeatureElement* feature);

        /// Remove the node from the flow scene.
        void removeNodeFromFlowScene(ElementId elementId);

        /// Move the node in the flow scene.
        void moveNodeInFlowScene(ElementId elementId, const UiPosition& newPosition);

        /// Resize the node in the flow scene.
        void resizeNodeInFlowScene(ElementId elementId, const UiSize& newSize);

        /// Update the contents of the node.
        void onNodeContentChanged(ElementId elementId);

        /// Make a connection description from a flow scene connection.
        ConnectionDescription describeConnection(babelwires::AccessModelScope& scope, const FlowSceneConnectionInfo& c);

      private slots:

        void onNodeCreated(QtNodes::Node& n);
        void onNodeDeleted(QtNodes::Node& n);
        void onNodeMoved(QtNodes::Node& n, const QPointF& newLocation);
        void onNodeResized(QtNodes::Node& n, const QSize& newSize);
        void onConnectionCreated(const QtNodes::Connection& c);
        void onConnectionDeleted(const QtNodes::Connection& c);
        void onConnectionUpdated(const QtNodes::Connection& c);
        void onSelectionChanged();

        // When Qt is idle.
        void onIdle();

      private:
        /// Triggered internally, when above slots observe a connection is made between two nodes.
        void onConnectionConnected(const QtNodes::Connection& c);

        /// Triggered internally, when above slots observe a connection is broken between two nodes.
        void onConnectionDisconnected(const QtNodes::Connection& c);

        /// Triggered internally, when a connection is moved to a different port.
        void onConnectionAdjusted(const QtNodes::Connection& c);

      private:
        friend AccessModelScope;
        friend ModifyModelScope;

        Project& m_project;
        CommandManager<Project>& m_commandManager;
        UiProjectContext& m_projectContext;

        QtNodes::FlowScene* m_flowScene = nullptr;

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

        /// Observe changes in the project, and fire signals.
        ProjectObserver m_projectObserver;

        /// Subscribe to the project observer's signals.
        std::vector<SignalSubscription> m_projectObserverSubscriptions;

        // Information about the currently connected connections.
        MultiKeyMap<const QtNodes::Connection*, ConnectionDescription, FlowSceneConnectionInfo> m_connectedConnections;

        /// Find the node in the scene that corresponds to an element id.
        std::unordered_map<ElementId, QtNodes::Node*> m_nodeFromElementId;

        /// Most commands are scheduled to run when the UI is idle, rather than performed synchronously.
        /// One reason is that the processing of some commands modify the UI, causing inconsistencies
        /// and crashes.
        std::unique_ptr<Command<Project>> m_scheduledCommand;

        bool m_newNodesShouldBeSelected = false;
    };

} // namespace babelwires
