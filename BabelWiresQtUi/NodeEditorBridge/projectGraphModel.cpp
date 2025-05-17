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

#include <QtNodes/StyleCollection>

#include <QTimer>

class babelwires::ProjectGraphModel::StateScope final {
  public:
    StateScope(ProjectGraphModel& projectGraphModel, State newState)
        : m_projectGraphModel(projectGraphModel)
        , m_previousState(projectGraphModel.m_state) {
        projectGraphModel.m_state = newState;
    }

    ~StateScope() { m_projectGraphModel.m_state = m_previousState; }

  private:
    ProjectGraphModel& m_projectGraphModel;
    State m_previousState;
};

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
        [this](NodeId nodeId, const UiPosition& uiPosition) { Q_EMIT nodePositionUpdated(nodeId); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasResized.subscribe(
        [this](NodeId nodeId, const UiSize& newSize) { Q_EMIT nodeUpdated(nodeId); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasAdded.subscribe(
        [this](const ConnectionDescription& connection) { addConnectionToFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasRemoved.subscribe(
        [this](const ConnectionDescription& connection) { removeConnectionFromFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(
        m_projectObserver.m_contentWasChanged.subscribe([this](NodeId nodeId) { Q_EMIT nodeUpdated(nodeId); }));
}

babelwires::ProjectGraphModel::~ProjectGraphModel() = default;

QtNodes::ConnectionId
babelwires::ProjectGraphModel::createConnectionIdFromConnectionDescription(const AccessModelScope& scope,
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
babelwires::ProjectGraphModel::createConnectionDescriptionFromConnectionId(const AccessModelScope& scope,
                                                                           const QtNodes::ConnectionId& connectionId) {
    const auto sourceIt = m_nodeModels.find(connectionId.outNodeId);
    assert(sourceIt != m_nodeModels.end());
    const auto targetIt = m_nodeModels.find(connectionId.inNodeId);
    assert(targetIt != m_nodeModels.end());
    const Path sourcePath = sourceIt->second->getPathAtPort(scope, QtNodes::PortType::Out, connectionId.outPortIndex);
    const Path targetPath = targetIt->second->getPathAtPort(scope, QtNodes::PortType::In, connectionId.inPortIndex);
    return ConnectionDescription{static_cast<NodeId>(connectionId.outNodeId), std::move(sourcePath),
                                 static_cast<NodeId>(connectionId.inNodeId), std::move(targetPath)};
}

void babelwires::ProjectGraphModel::addConnectionToFlowScene(const ConnectionDescription& connection) {
    assert(m_state == State::ProcessingModelChanges);
    AccessModelScope scope(*this);
    const QtNodes::ConnectionId connectionId = createConnectionIdFromConnectionDescription(scope, connection);
    Q_EMIT connectionCreated(connectionId);
    const auto sourceIt = m_nodeModels.find(connectionId.outNodeId);
    assert(sourceIt != m_nodeModels.end());
    sourceIt->second->addOutConnection(connectionId, connection);
    const auto targetIt = m_nodeModels.find(connectionId.inNodeId);
    assert(targetIt != m_nodeModels.end());
    targetIt->second->addInConnection(connectionId, connection);
}

void babelwires::ProjectGraphModel::removeConnectionFromFlowScene(const ConnectionDescription& connection) {
    assert(m_state == State::ProcessingModelChanges);
    AccessModelScope scope(*this);
    const auto sourceIt = m_nodeModels.find(connection.m_sourceId);
    assert(sourceIt != m_nodeModels.end());
    sourceIt->second->removeOutConnection(connection);
    const auto targetIt = m_nodeModels.find(connection.m_targetId);
    assert(targetIt != m_nodeModels.end());
    const QtNodes::ConnectionId connectionId = targetIt->second->removeInConnection(connection);
    Q_EMIT connectionDeleted(connectionId);
}

void babelwires::ProjectGraphModel::addNodeToFlowScene(const Node* node) {
    assert(m_state == State::ProcessingModelChanges);
    const NodeId nodeId = node->getNodeId();
#ifndef NDEBUG
    const auto resultPair =
#endif
        m_nodeModels.insert(std::make_pair(nodeId, std::make_unique<NodeNodeModel>(*this, nodeId)));
    assert(resultPair.second && "There's already a nodeModel for that nodeId");
    Q_EMIT nodeCreated(nodeId);
    // We don't need to add connections here: The project observer will fire signals about them.
}

void babelwires::ProjectGraphModel::removeNodeFromFlowScene(NodeId nodeId) {
    assert(m_state == State::ProcessingModelChanges);
    auto it = m_nodeModels.find(nodeId);
    assert((it != m_nodeModels.end()) && "Trying to remove unrecognized node");
    m_nodeModels.erase(it);
    Q_EMIT nodeDeleted(nodeId);
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

bool babelwires::ProjectGraphModel::connectionPossible(QtNodes::ConnectionId const connectionId) const {
    const auto targetIt = m_nodeModels.find(connectionId.inNodeId);
    assert(targetIt != m_nodeModels.end());
    const NodeNodeModel& targetNodeModel = *targetIt->second;

    const auto sourceIt = m_nodeModels.find(connectionId.inNodeId);
    assert(sourceIt != m_nodeModels.end());
    const NodeNodeModel& sourceNodeModel = *sourceIt->second;

    AccessModelScope scope(*this);
    const QtNodes::NodeDataType sourceDataType = sourceNodeModel.dataType(scope, QtNodes::PortType::Out, connectionId.outPortIndex);
    const QtNodes::NodeDataType targetDataType = targetNodeModel.dataType(scope, QtNodes::PortType::In, connectionId.inPortIndex);

    // TODO: Use Subtyping.
    return !sourceDataType.id.isEmpty() && !targetDataType.id.isEmpty() && sourceDataType.id == targetDataType.id;
}

void babelwires::ProjectGraphModel::addConnection(QtNodes::ConnectionId const connectionId) {
    assert(m_state == State::ListeningToFlowScene);
    AccessModelScope scope(*this);
    auto connectionDescription = createConnectionDescriptionFromConnectionId(scope, connectionId);
    scheduleCommand(connectionDescription.getConnectionCommand());
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
            return QPointF{static_cast<qreal>(position.m_x), static_cast<qreal>(position.m_y)};
        }
        case QtNodes::NodeRole::Size:
            return nodeModel.getCachedSize();
        case QtNodes::NodeRole::CaptionVisible:
            return true;
        case QtNodes::NodeRole::Caption: {
            AccessModelScope scope(*this);
            return nodeModel.caption(scope);
        }
        case QtNodes::NodeRole::InPortCount: {
            AccessModelScope scope(*this);
            return nodeModel.nPorts(scope, QtNodes::PortType::In);
        }
        case QtNodes::NodeRole::OutPortCount: {
            AccessModelScope scope(*this);
            return nodeModel.nPorts(scope, QtNodes::PortType::Out);
        }
        case QtNodes::NodeRole::Widget: {
            // TODO fromValue won't accept a const QWidget*
            QWidget* widget = const_cast<QWidget*>(nodeModel.getEmbeddedWidget());
            return QVariant::fromValue(widget);
        }
        case QtNodes::NodeRole::Style: {
            auto style = QtNodes::StyleCollection::nodeStyle();
            return style.toJson().toVariantMap();
        }

        // Unimplemented.
        case QtNodes::NodeRole::InternalData:
        default:
            assert(false && "Not expecting this kind of node data to be queried");
            return {};
    }
}

QtNodes::NodeFlags babelwires::ProjectGraphModel::nodeFlags(QtNodes::NodeId nodeId) const {
    return QtNodes::NodeFlag::NoFlags;
}

bool babelwires::ProjectGraphModel::setNodeData(QtNodes::NodeId nodeId, QtNodes::NodeRole role, QVariant value) {
    switch (role) {
        case QtNodes::NodeRole::Position:
            nodeMoved(nodeId, value.value<QPointF>());
            return true;
        case QtNodes::NodeRole::Size:
            nodeResized(nodeId, value.value<QSize>());
            return true;
        default:
            assert(false && "Not expecting this kind of node data to change");
            return false;
    }
}

QVariant babelwires::ProjectGraphModel::portData(QtNodes::NodeId nodeId, QtNodes::PortType portType,
                                                 QtNodes::PortIndex index, QtNodes::PortRole role) const {
    switch (role) {
        case QtNodes::PortRole::DataType: { ///< `QString` describing the port data type.
            const auto it = m_nodeModels.find(nodeId);
            assert(it != m_nodeModels.end());
            const NodeNodeModel& nodeModel = *it->second;
            AccessModelScope scope(*this);
            return QVariant::fromValue(nodeModel.dataType(scope, portType, index));
        }
        case QtNodes::PortRole::ConnectionPolicyRole: ///< `enum` ConnectionPolicyRole
            // TODO Not sure what option is needed here. Only collapsing should create many in connections at a single
            // port.
            return QVariant::fromValue(QtNodes::ConnectionPolicy::Many);
        case QtNodes::PortRole::CaptionVisible: ///< `bool` for caption visibility.
            return false;
        case QtNodes::PortRole::Caption: ///< `QString` for port caption.
            return QString();
        default:
        case QtNodes::PortRole::Data: ///< `std::shared_ptr<NodeData>`.
            assert(false);
            return {};
    };
}

void babelwires::ProjectGraphModel::nodeMoved(QtNodes::NodeId nodeId, const QPointF& newLocation) {
    if (m_state != State::ListeningToFlowScene) {
        return;
    }
    const auto it = m_nodeModels.find(nodeId);
    assert(it != m_nodeModels.end());
    NodeNodeModel& nodeNodeModel = *it->second;

    AccessModelScope scope(*this);
    const Node* node = scope.getProject().getNode(nodeId);
    assert(node && "The node should already be in the project");
    const UiPosition& uiPosition = node->getNodeData().m_uiData.m_uiPosition;
    UiPosition newPosition{static_cast<UiCoord>(newLocation.x()), static_cast<UiCoord>(newLocation.y())};
    if (uiPosition != newPosition) {
        std::string commandName = "Move " + nodeNodeModel.caption(scope).toStdString();
        scheduleCommand(std::make_unique<MoveNodeCommand>(commandName, nodeId, newPosition));
    }
}

void babelwires::ProjectGraphModel::nodeResized(QtNodes::NodeId nodeId, const QSize& newSize) {
    const auto it = m_nodeModels.find(nodeId);
    assert(it != m_nodeModels.end());
    NodeNodeModel& nodeNodeModel = *it->second;
    nodeNodeModel.setCachedSize(newSize);
}

void babelwires::ProjectGraphModel::scheduleCommand(std::unique_ptr<Command<Project>> command) {
    //assert(m_state != State::ProcessingModelChanges);
    if (m_scheduledCommand) {
        assert(m_scheduledCommand->shouldSubsume(*command, false) && "Commands scheduled together should subsume");
        m_scheduledCommand->subsume(std::move(command));
    } else {
        m_scheduledCommand = std::move(command);
        // Schedule the execution of the command.
        QTimer::singleShot(0, this, &ProjectGraphModel::onIdle);
    }
}

bool babelwires::ProjectGraphModel::executeCommandSynchronously(std::unique_ptr<Command<Project>> command) {
    assert(m_state != State::ProcessingModelChanges);
    ModifyModelScope scope(*this);
    std::unique_ptr<Command<Project>> commandPtr = std::move(command);
    return scope.getCommandManager().executeAndStealCommand(commandPtr);
}

void babelwires::ProjectGraphModel::processAndHandleModelChanges() {
    assert(m_state != State::ProcessingModelChanges);
    // This is called from ~ModifyModelScope, so no scope is needed here.
    m_project.process();
    {
        StateScope stateScope(*this, State::ProcessingModelChanges);
        m_projectObserver.interpretChangesAndFireSignals();
    }
    m_project.clearChanges();

    // Reset this if it was set.
    if (m_disconnectAfterProcessing) {
        QObject::disconnect(m_disconnectAfterProcessing);
        m_disconnectAfterProcessing = QMetaObject::Connection();
    }
}

void babelwires::ProjectGraphModel::onIdle() {
    assert(m_state == State::ListeningToFlowScene);
    if (m_scheduledCommand) {
        ModifyModelScope scope(*this);
        std::unique_ptr<Command<Project>> scheduledCommand = std::move(m_scheduledCommand);
        scope.getCommandManager().executeAndStealCommand(scheduledCommand);
    }
}

babelwires::MainWindow* babelwires::ProjectGraphModel::getMainWindow() const {
    return m_mainWindow;
}

QGraphicsView* babelwires::ProjectGraphModel::getFlowGraphWidget() {
    return m_flowGraphWidget;
}

void babelwires::ProjectGraphModel::setWidgets(MainWindow* mainWindow, QGraphicsView* flowGraphWidget) {
    m_mainWindow = mainWindow;
    m_flowGraphWidget = flowGraphWidget;
}

const babelwires::UiProjectContext& babelwires::ProjectGraphModel::getContext() const {
    return m_projectContext;
}

babelwires::ProjectData babelwires::ProjectGraphModel::getDataFromSelectedNodes(const std::vector<NodeId>& selectedNodes) const {
    ProjectData projectData;
    AccessModelScope scope(*this);
    const Project& project = scope.getProject();
    projectData.m_projectId = project.getProjectId();

    for (NodeId nodeId : selectedNodes) {
        const Node* const node = project.getNode(nodeId);
        assert(node && "The node is not in the project");
        projectData.m_nodes.emplace_back(node->extractNodeData());
    }

    return projectData;
}

void babelwires::ProjectGraphModel::disconnectAfterProcessing(QMetaObject::Connection connection) {
    assert(!m_disconnectAfterProcessing);
    assert(connection);
    m_disconnectAfterProcessing = std::move(connection);
}
