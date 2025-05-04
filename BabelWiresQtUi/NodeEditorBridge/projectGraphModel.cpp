/**
 *
 * Copyright (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/nodeNodeModel.hpp>

#include <BabelWiresLib/Project/project.hpp>

babelwires::ProjectGraphModel::ProjectGraphModel(Project& project, CommandManager<Project>& commandManager,
                                                 UiProjectContext& projectContext)
    : m_project(project)
    , m_commandManager(commandManager)
    , m_projectContext(projectContext)
    , m_state(State::ListeningToFlowScene)
    , m_projectObserver(project) {
    m_projectObserverSubscriptions.emplace_back(
        m_projectObserver.m_nodeWasAdded.subscribe([this](const Node* node) { addNodeToFlowScene(node); }));

    m_projectObserverSubscriptions.emplace_back(
        m_projectObserver.m_nodeWasRemoved.subscribe([this](NodeId nodeId) { removeNodeFromFlowScene(nodeId); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasMoved.subscribe(
        [this](NodeId nodeId, const UiPosition& uiPosition) { nodePositionUpdated(nodeId); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasResized.subscribe(
        [this](NodeId nodeId, const UiSize& newSize) { nodeUpdated(nodeId); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasAdded.subscribe(
        [this](const ConnectionDescription& connection) { addConnectionToFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasRemoved.subscribe(
        [this](const ConnectionDescription& connection) { removeConnectionFromFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(
        m_projectObserver.m_contentWasChanged.subscribe([this](NodeId nodeId) { nodeUpdated(nodeId); }));
}

QtNodes::ConnectionId
babelwires::ProjectGraphModel::createConnectionIdFromConnection(AccessModelScope& scope,
                                                                ConnectionDescription& connection) {
    const auto sourceIt = m_nodeModels.find(connection.m_sourceId);
    assert(sourceIt != m_nodeModels.end());
    const auto targetIt = m_nodeModels.find(connection.m_targetId);
    assert(targetIt != m_nodeModels.end());
    QtNodes::PortIndex portIndexOut =
        sourceIt.second->getPortAtPath(scope, QtNodes::PortType::Out, connection.m_sourcePath);
    QtNodes::PortIndex portIndexIn =
        targetIt.second->getPortAtPath(scope, QtNodes::PortType::In, connection.m_targetPath);
    return QtNodes::ConnectionId{connection.m_sourceId, portIndexOut, connection.m_targetId, portIndexIn};
}

ConnectionDescription
babelwires::ProjectGraphModel::createConnectionDescriptionFromConnectionId(AccessModelScope& scope,
                                                                           const QtNodes::ConnectionId& connectionId) {
    const auto sourceIt = m_nodeModels.find(connectionId.outPortId);
    assert(sourceIt != m_nodeModels.end());
    const auto targetIt = m_nodeModels.find(connectionId.inPortId);
    assert(targetIt != m_nodeModels.end());
    const Path sourcePath = sourceIt.second->getPathAtPort(scope, QtNodes::PortType::Out, c.outPortIndex);
    const Path targetPath = targetIt.second->getPathAtPort(scope, QtNodes::PortType::In, c.inPortIndex);
    return ConnectionDescription{connectionId.m_sourceId, std::move(sourcePath), connectionId.m_targetId,
                                 std::move(targetPath)};
}

void babelwires::ProjectGraphModel::addToConnectionCache(const ConnectionId& connectionId) {
    const auto sourceIt = m_nodeModels.find(connectionId.outPortId);
    assert(sourceIt != m_nodeModels.end());
    sourceIt.second->addOutConnection(connectionId);
    const auto targetIt = m_nodeModels.find(connectionId.inPortId);
    assert(targetIt != m_nodeModels.end());
    targetIt.second->addInConnection(connectionId);
}

void babelwires::ProjectGraphModel::removeFromConnectionCache(const ConnectionId& connectionId) {
    const auto sourceIt = m_nodeModels.find(connectionId.outPortId);
    assert(sourceIt != m_nodeModels.end());
    sourceIt.second->removeOutConnection(connectionId);
    const auto targetIt = m_nodeModels.find(connectionId.inPortId);
    assert(targetIt != m_nodeModels.end());
    targetIt.second->removeInConnection(connectionId);
}

void babelwires::ProjectGraphModel::addConnectionToFlowScene(const ConnectionDescription& connection) {
    AccessModelScope scope(*this);
    const QtNodes::ConnectionId connectionId = createConnectionIdFromConnection(scope, connection);
    connectionCreated(connectionId);
    addToConnectionCache(connectionId);
}

void babelwires::ProjectGraphModel::removeConnectionFromFlowScene(const ConnectionDescription& connection) {
    removeFromConnectionCache(connectionId);
    AccessModelScope scope(*this);
    const QtNodes::ConnectionId connectionId = createConnectionIdFromConnection(scope, connection);
    removeFromConnectionCache(connectionId);
    connectionDeleted(connectionId);
}

void babelwires::ProjectGraphModel::addNodeToFlowScene(const Node* node) {
    const NodeId nodeId = node->getNodeId();
#ifndef NDEBUG
    const auto resultPair =
#endif
        m_nodeModels.insert(std::make_pair(nodeId, std::make_unique<NodeNodeModel>(*this, nodeId)));
    assert(resultPair.second && "There's already a nodeModel for that nodeId");
    nodeCreated(nodeId);
    // We don't need to add connections here: The project observer will fire signals about them.

    /* TODO Would need access to graphics objects.
    // Place new nodes at the very top.
    newNode.nodeGraphicsObject().setZValue(std::numeric_limits<qreal>::max());
    if (m_newNodesShouldBeSelected) {
        newNode.nodeGraphicsObject().setSelected(true);
    }
    */
}

void babelwires::ProjectGraphModel::removeNodeFromFlowScene(NodeId nodeId) {
    auto it = m_nodeModels.find(nodeId);
    assert((it != m_nodeModels.end()) && "Trying to remove unrecognized node");
    m_nodeModels.erase(it);
    nodeDeleted(nodeId);
}

std::unordered_set<NodeId> babelwires::ProjectGraphModel::allNodeIds() const {
    AccessModelScope scope(this);
    const auto& nodes = scope.getProject().getNodes();
    std::unordered_set<NodeId> nodeIds;
    nodeIds.reserve(nodes.size());
    for (const auto& pair : nodes) {
        nodeIds.insert(pair.first);
    }
    return nodeIds;
}

std::unordered_set<ConnectionId> babelwires::ProjectGraphModel::allConnectionIds(QtNodes::NodeId const nodeId) const {
    // Because rows can be collapsed, the connectionInfo structure in the project cannot be used for this.
    const auto nodeIt = m_nodeModels.find(nodeId);
    assert(nodeIt != m_nodeModels.end());
    return *nodeIt.second->getAllConnectionIds();
}

std::unordered_set<ConnectionId> babelwires::ProjectGraphModel::connections(QtNodes::NodeId nodeId, PortType portType,
                                                                            PortIndex index) const {
    const auto nodeIt = m_nodeModels.find(nodeId);
    assert(nodeIt != m_nodeModels.end());
    return nodeIt.second->getConnections(portType, index);
};

bool babelwires::ProjectGraphModel::connectionExists(ConnectionId const connectionId) const {
    const auto nodeIt = m_nodeModels.find(connectionId.inPortId);
    assert(nodeIt != m_nodeModels.end());
    return nodeIt.second->isInConnection();
};

bool babelwires::ProjectGraphModel::connectionPossible(ConnectionId const connectionId) const {
    // TODO
    return true;
};

void babelwires::ProjectGraphModel::addConnection(ConnectionId const connectionId) {
    addToConnectionCache(connectionId);
    auto connectionDescription = createConnectionDescriptionFromConnectionId(connectionId);
    scheduleCommand(connectionDescription.getConnectionCommand());
    // TODO
    m_projectObserver.ignoreAddedConnection(std::move(modelInfo));
};

bool babelwires::ProjectGraphModel::nodeExists(QtNodes::NodeId const nodeId) const {
    return (m_nodeModels.find(nodeId) != m_nodeModels.end());
};

QVariant babelwires::ProjectGraphModel::nodeData(QtNodes::NodeId nodeId, NodeRole role) const {
    const auto it = m_nodeModels.find(nodeId);
    assert(it != m_nodeModels.end());
    const NodeNodeModel& nodeModel = *it.second;
    switch (role) {
        case NodeRole::Type:
            // TODO This should maybe be something like processor / value / source / target.
            return "TODO type";
        case NodeRole::Position: {
            AccessModelScope scope(*this);
            const Node* const node = scope.getProject().getNode(nodeId);
            const UiPosition position = node.getUiPosition();
            return QPointF{position.m_x, position.m_y};
        }
        case NodeRole::Size: {
            AccessModelScope scope(*this);
            const Node* const node = scope.getProject().getNode(nodeId);
            const UiSize size = node.getUiSize();
            return QSize{size.m_width, nodeModel.embeddedWidget()->height()};
        }
        case NodeRole::CaptionVisible:
            return true;
        case NodeRole::Caption:
            return nodeModel.caption();
        case NodeRole::InPortCount:
            return nodeModel.nPorts(QtNodes::PortType::In);
        case NodeRole::OutPortCount:
            return nodeModel.nPorts(QtNodes::PortType::Out);
        case NodeRole::Widget:
            return nodeModel.embeddedWidget();

        // Unimplemented.
        case NodeRole::Style:
        case NodeRole::InternalData:
        default:
            assert(false && "Not expecting this kind of node data to be queried");
            return {};
    }
};

QtNodes::NodeFlags babelwires::ProjectGraphModel::nodeFlags(NodeId nodeId) const {
    // TODO Distinguish between horizontal and vertical resizing.
    return QtNodes::NodeFlag::Resizable;
};

bool babelwires::ProjectGraphModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value) {
    switch (role) {
        case NodeRole::Position:
            nodeMoved(nodeId, value.value<QPointF>());
        case NodeRole::Size:
            nodeResized(nodeId, value.value<QSize>());
        default:
            assert(false && "Not expecting this kind of node data to change");
            return false;
    }
};

QVariant babelwires::ProjectGraphModel::portData(NodeId nodeId, PortType portType, PortIndex index,
                                                 PortRole role) const {};

bool babelwires::ProjectGraphModel::deleteConnection(ConnectionId const connectionId) {
    removeFromConnectionCache(connectionId);
    AccessModelScope scope(*this);
    auto connectionDescription = createConnectionDescriptionFromConnectionId(scope, connectionId);
    scheduleCommand(connectionDescription.getDisconnectionCommand());
    // TODO
    m_projectObserver.ignoreRemovedConnection(std::move(modelInfo));
};

bool babelwires::ProjectGraphModel::deleteNode(NodeId const nodeId) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            auto it = m_nodeModels.find(nodeId);
            assert(it != m_nodeModels.end());
            // This assert is non-essential, but it helps establish assumptions about the behaviour of the flow scene.
            assert(it.second->getAllConnectionIds().size() == 0 && "Node was removed while there were active connections");
            m_nodeModels.erase(it);
            scheduleCommand(std::make_unique<RemoveNodeCommand>("Remove node", nodeId));
        }
        case State::ProcessingModelChanges: {
            // Nothing to do.
            break;
        }
        default: {
            assert(false && "Unexpected state");
        }
    }
};

void babelwires::ProjectGraphModel::onNodeMoved(QtNodes::NodeId nodeId, const QPointF& newLocation) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            const auto it = m_nodeModels.find(nodeId);
            assert(it != m_nodeModels.end());
            NodeNodeModel& nodeNodeModel = *it->second;
            auto& nodeNodeModel = dynamic_cast<NodeNodeModel&>(dataModel);

            AccessModelScope scope(*this);
            const Node* node = scope.getProject().getNode(nodeId);
            assert(node && "The node should already be in the project");
            const UiPosition& uiPosition = node->getNodeData().m_uiData.m_uiPosition;
            UiPosition newPosition{static_cast<UiCoord>(newLocation.x()), static_cast<UiCoord>(newLocation.y())};
            if (uiPosition != newPosition) {
                std::string commandName = "Move " + nodeNodeModel.caption().toStdString();
                scheduleCommand(std::make_unique<MoveNodeCommand>(commandName, nodeId, newPosition));
                m_projectObserver.ignoreMovedNode(nodeId);
            }
            break;
        }
        case State::ProcessingModelChanges: {
            // Nothing to do.
            break;
        }
        default: {
            assert(false && "Unexpected state");
        }
    }
}

void babelwires::ProjectGraphModel::onNodeResized(QtNodes::NodeId nodeId, const QSize& newSize) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            const auto it = m_nodeModels.find(nodeId);
            assert(it != m_nodeModels.end());
            NodeNodeModel& nodeNodeModel = *it->second;
            auto& nodeNodeModel = dynamic_cast<NodeNodeModel&>(dataModel);

            AccessModelScope scope(*this);
            const Node* node = scope.getProject().getNode(nodeId);
            assert(node && "The node should already be in the project");
            const int currentWidth = node->getUiSize().m_width;
            const int newWidth = nodeNodeModel.embeddedWidget()->size().width();
            if (newWidth != currentWidth) {
                std::string commandName = "Resize " + nodeNodeModel.caption().toStdString();
                scheduleCommand(std::make_unique<ResizeNodeCommand>(commandName, nodeId, UiSize{newWidth}));
                m_projectObserver.ignoreResizedNode(nodeId);
            }
            break;
        }
        case State::ProcessingModelChanges: {
            // Nothing to do.
            break;
        }
        default: {
            assert(false && "Unexpected state");
        }
    }
}

void babelwires::ProjectGraphModel::scheduleCommand(std::unique_ptr<Command<Project>> command) {
    if (m_scheduledCommand) {
        assert(m_scheduledCommand->shouldSubsume(*command, false) && "Commands scheduled together should subsume");
        m_scheduledCommand->subsume(std::move(command));
    } else {
        m_scheduledCommand = std::move(command);
        // Schedule the execution of the command.
        QTimer::singleShot(0, this, &ProjectBridge::onIdle);
    }
}

void babelwires::ProjectGraphModel::processAndHandleModelChanges() {
    // This is called from ~ModifyModelScope, so no scope is needed here.
    m_project.process();

    // Below we modify the UI, but don't want the model to react, so temporarily use ProcessingModelChanges.
    const State backedUpState = m_state;
    m_state = State::ProcessingModelChanges;

    m_projectObserver.interpretChangesAndFireSignals();

    m_project.clearChanges();

    m_state = backedUpState;

    // Reset this if it was set.
    m_newNodesShouldBeSelected = false;
}

void babelwires::ProjectGraphModel::onIdle() {
    if (m_scheduledCommand) {
        ModifyModelScope scope(*this);
        std::unique_ptr<Command<Project>> scheduledCommand = std::move(m_scheduledCommand);
        scope.getCommandManager().executeAndStealCommand(scheduledCommand);
    }
}

babelwires::MainWindow* babelwires::ProjectGraphModel::getMainWindow() const {
    return m_mainWindow;
}

void babelwires::ProjectGraphModel::setMainWindow(MainWindow* mainWindow) {
    m_mainWindow = mainWindow;
}
