/**
 *
 * Copyright (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 */
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeNodeCommand.hpp>
#include <BabelWiresLib/Project/Commands/resizeNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/nodeNodeModel.hpp>

#include <BabelWiresLib/Project/project.hpp>

#include <QTimer>

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
babelwires::ProjectGraphModel::createConnectionIdFromConnectionDescription(AccessModelScope& scope,
                                                                           const ConnectionDescription& connection) {
    const auto sourceIt = m_nodeModels.find(connection.m_sourceId);
    assert(sourceIt != m_nodeModels.end());
    const auto targetIt = m_nodeModels.find(connection.m_targetId);
    assert(targetIt != m_nodeModels.end());
    QtNodes::PortIndex portIndexOut =
        sourceIt->second->getPortAtPath(scope, QtNodes::PortType::Out, connection.m_sourcePath);
    QtNodes::PortIndex portIndexIn =
        targetIt->second->getPortAtPath(scope, QtNodes::PortType::In, connection.m_targetPath);
    return QtNodes::ConnectionId{connection.m_sourceId, portIndexOut, connection.m_targetId, portIndexIn};
}

babelwires::ConnectionDescription
babelwires::ProjectGraphModel::createConnectionDescriptionFromConnectionId(AccessModelScope& scope,
                                                                           const QtNodes::ConnectionId& connectionId) {
    const auto sourceIt = m_nodeModels.find(connectionId.outNodeId);
    assert(sourceIt != m_nodeModels.end());
    const auto targetIt = m_nodeModels.find(connectionId.inNodeId);
    assert(targetIt != m_nodeModels.end());
    const Path sourcePath = sourceIt->second->getPathAtPort(scope, QtNodes::PortType::Out, connectionId.outPortIndex);
    const Path targetPath = targetIt->second->getPathAtPort(scope, QtNodes::PortType::In, connectionId.inPortIndex);
    return ConnectionDescription{connectionId.outNodeId, std::move(sourcePath), connectionId.inNodeId,
                                 std::move(targetPath)};
}

void babelwires::ProjectGraphModel::addToConnectionCache(const QtNodes::ConnectionId& connectionId) {
    const auto sourceIt = m_nodeModels.find(connectionId.outNodeId);
    assert(sourceIt != m_nodeModels.end());
    sourceIt->second->addOutConnection(connectionId);
    const auto targetIt = m_nodeModels.find(connectionId.inNodeId);
    assert(targetIt != m_nodeModels.end());
    targetIt->second->addInConnection(connectionId);
}

void babelwires::ProjectGraphModel::removeFromConnectionCache(const QtNodes::ConnectionId& connectionId) {
    const auto sourceIt = m_nodeModels.find(connectionId.outNodeId);
    assert(sourceIt != m_nodeModels.end());
    sourceIt->second->removeOutConnection(connectionId);
    const auto targetIt = m_nodeModels.find(connectionId.inNodeId);
    assert(targetIt != m_nodeModels.end());
    targetIt->second->removeInConnection(connectionId);
}

void babelwires::ProjectGraphModel::addConnectionToFlowScene(const ConnectionDescription& connection) {
    AccessModelScope scope(*this);
    const QtNodes::ConnectionId connectionId = createConnectionIdFromConnectionDescription(scope, connection);
    connectionCreated(connectionId);
    addToConnectionCache(connectionId);
}

void babelwires::ProjectGraphModel::removeConnectionFromFlowScene(const ConnectionDescription& connection) {
    AccessModelScope scope(*this);
    const QtNodes::ConnectionId connectionId = createConnectionIdFromConnectionDescription(scope, connection);
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

std::unordered_set<QtNodes::NodeId> babelwires::ProjectGraphModel::allNodeIds() const {
    std::unordered_set<QtNodes::NodeId> nodeIds;
    nodeIds.reserve(m_nodeModels.size());
    for (const auto& pair : m_nodeModels) {
        nodeIds.insert(pair.first);
    }
    return nodeIds;
}

std::unordered_set<QtNodes::ConnectionId>
babelwires::ProjectGraphModel::allConnectionIds(QtNodes::NodeId const nodeId) const {
    // Because rows can be collapsed, the connectionInfo structure in the project cannot be used for this.
    const auto nodeIt = m_nodeModels.find(nodeId);
    assert(nodeIt != m_nodeModels.end());
    return nodeIt->second->getAllConnectionIds();
}

std::unordered_set<QtNodes::ConnectionId> babelwires::ProjectGraphModel::connections(QtNodes::NodeId nodeId,
                                                                                     QtNodes::PortType portType,
                                                                                     QtNodes::PortIndex index) const {
    const auto nodeIt = m_nodeModels.find(nodeId);
    assert(nodeIt != m_nodeModels.end());
    return nodeIt->second->getConnections(portType, index);
}

bool babelwires::ProjectGraphModel::connectionExists(QtNodes::ConnectionId const connectionId) const {
    const auto nodeIt = m_nodeModels.find(connectionId.inNodeId);
    assert(nodeIt != m_nodeModels.end());
    return nodeIt->second->isInConnection(connectionId);
}

bool babelwires::ProjectGraphModel::connectionPossible(QtNodes::ConnectionId const connectionId) const {
    // TODO
    return true;
}

void babelwires::ProjectGraphModel::addConnection(QtNodes::ConnectionId const connectionId) {
    addToConnectionCache(connectionId);
    AccessModelScope scope(*this);
    auto connectionDescription = createConnectionDescriptionFromConnectionId(scope, connectionId);
    scheduleCommand(connectionDescription.getConnectionCommand());
    // TODO
    m_projectObserver.ignoreAddedConnection(std::move(connectionDescription));
}

bool babelwires::ProjectGraphModel::nodeExists(QtNodes::NodeId const nodeId) const {
    return (m_nodeModels.find(nodeId) != m_nodeModels.end());
}

QVariant babelwires::ProjectGraphModel::nodeData(QtNodes::NodeId nodeId, QtNodes::NodeRole role) const {
    const auto it = m_nodeModels.find(nodeId);
    assert(it != m_nodeModels.end());
    const NodeNodeModel& nodeModel = *it->second;
    switch (role) {
        case QtNodes::NodeRole::Type:
            // TODO This should maybe be something like processor / value / source / target.
            return "TODO type";
        case QtNodes::NodeRole::Position: {
            AccessModelScope scope(*this);
            const Node* const node = scope.getProject().getNode(nodeId);
            const UiPosition position = node->getUiPosition();
            return QPointF{position.m_x, position.m_y};
        }
        case QtNodes::NodeRole::Size: {
            AccessModelScope scope(*this);
            const Node* const node = scope.getProject().getNode(nodeId);
            const UiSize size = node->getUiSize();
            return QSize{size.m_width, nodeModel.embeddedWidget()->height()};
        }
        case QtNodes::NodeRole::CaptionVisible:
            return true;
        case QtNodes::NodeRole::Caption:
            return nodeModel.caption();
        case QtNodes::NodeRole::InPortCount:
            return nodeModel.nPorts(QtNodes::PortType::In);
        case QtNodes::NodeRole::OutPortCount:
            return nodeModel.nPorts(QtNodes::PortType::Out);
        case QtNodes::NodeRole::Widget:
            return nodeModel.embeddedWidget();

        // Unimplemented.
        case QtNodes::NodeRole::Style:
        case QtNodes::NodeRole::InternalData:
        default:
            assert(false && "Not expecting this kind of node data to be queried");
            return {};
    }
}

QtNodes::NodeFlags babelwires::ProjectGraphModel::nodeFlags(QtNodes::NodeId nodeId) const {
    // TODO Distinguish between horizontal and vertical resizing.
    return QtNodes::NodeFlag::Resizable;
}

bool babelwires::ProjectGraphModel::setNodeData(QtNodes::NodeId nodeId, QtNodes::NodeRole role, QVariant value) {
    switch (role) {
        case QtNodes::NodeRole::Position:
            nodeMoved(nodeId, value.value<QPointF>());
        case QtNodes::NodeRole::Size:
            nodeResized(nodeId, value.value<QSize>());
        default:
            assert(false && "Not expecting this kind of node data to change");
            return false;
    }
}

QVariant babelwires::ProjectGraphModel::portData(QtNodes::NodeId nodeId, QtNodes::PortType portType,
                                                 QtNodes::PortIndex index, QtNodes::PortRole role) const {
    // TODO
    switch (role) {
        case QtNodes::PortRole::DataType: ///< `QString` describing the port data type.
            return "TODO";
        case QtNodes::PortRole::ConnectionPolicyRole: ///< `enum` ConnectionPolicyRole
            // TODO Not sure what option is needed here. Only collapsing should create many in connections at a single
            // port.
            return QVariant::fromValue(QtNodes::ConnectionPolicy::Many);
        case QtNodes::PortRole::CaptionVisible: ///< `bool` for caption visibility.
            return false;
        case QtNodes::PortRole::Caption: ///< `QString` for port caption.
            return "";
        default:
        case QtNodes::PortRole::Data: ///< `std::shared_ptr<NodeData>`.
            assert(false);
            return {};
    };
}

bool babelwires::ProjectGraphModel::deleteConnection(QtNodes::ConnectionId const connectionId) {
    removeFromConnectionCache(connectionId);
    AccessModelScope scope(*this);
    auto connectionDescription = createConnectionDescriptionFromConnectionId(scope, connectionId);
    scheduleCommand(connectionDescription.getDisconnectionCommand());
    // TODO
    m_projectObserver.ignoreRemovedConnection(std::move(connectionDescription));
}

bool babelwires::ProjectGraphModel::deleteNode(QtNodes::NodeId const nodeId) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            auto it = m_nodeModels.find(nodeId);
            assert(it != m_nodeModels.end());
            // This assert is non-essential, but it helps establish assumptions about the behaviour of the flow
            // scene.
            assert(it->second->getAllConnectionIds().size() == 0 &&
                   "Node was removed while there were active connections");
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
}

void babelwires::ProjectGraphModel::nodeMoved(QtNodes::NodeId nodeId, const QPointF& newLocation) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            const auto it = m_nodeModels.find(nodeId);
            assert(it != m_nodeModels.end());
            NodeNodeModel& nodeNodeModel = *it->second;

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

void babelwires::ProjectGraphModel::nodeResized(QtNodes::NodeId nodeId, const QSize& newSize) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            const auto it = m_nodeModels.find(nodeId);
            assert(it != m_nodeModels.end());
            NodeNodeModel& nodeNodeModel = *it->second;

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
        QTimer::singleShot(0, this, &ProjectGraphModel::onIdle);
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
