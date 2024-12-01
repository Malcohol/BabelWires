/**
 * The ProjectBridge maintains the correspondence between the FlowScene and the Project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresQtUi/ModelBridge/elementNodeModel.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/modifyModelScope.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/Commands/commandManager.hpp>
#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Commands/removeNodeCommand.hpp>
#include <BabelWiresLib/Project/Commands/resizeNodeCommand.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>

#include <nodes/Connection>
#include <nodes/FlowScene>

#include <QTimer>
#include <QtWidgets/QGraphicsView>

#include <cassert>

babelwires::ProjectBridge::ProjectBridge(Project& project, CommandManager<Project>& commandManager,
                                         UiProjectContext& projectContext)
    : m_project(project)
    , m_commandManager(commandManager)
    , m_projectContext(projectContext)
    , m_state(State::ListeningToFlowScene)
    , m_projectObserver(project) {
    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasAdded.subscribe(
        [this](const Node* element) { addNodeToFlowScene(element); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasRemoved.subscribe(
        [this](NodeId elementId) { removeNodeFromFlowScene(elementId); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasMoved.subscribe(
        [this](NodeId elementId, const UiPosition& uiPosition) { moveNodeInFlowScene(elementId, uiPosition); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasResized.subscribe(
        [this](NodeId elementId, const UiSize& newSize) { resizeNodeInFlowScene(elementId, newSize); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasAdded.subscribe(
        [this](const ConnectionDescription& connection) { addConnectionToFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasRemoved.subscribe(
        [this](const ConnectionDescription& connection) { removeConnectionFromFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_contentWasChanged.subscribe(
        [this](NodeId elementId) { onNodeContentChanged(elementId); }));
}

babelwires::ProjectBridge::~ProjectBridge() {
    disconnect(m_flowScene, &QtNodes::FlowScene::nodeCreated, this, &ProjectBridge::onNodeCreated);
    disconnect(m_flowScene, &QtNodes::FlowScene::nodeDeleted, this, &ProjectBridge::onNodeDeleted);
    disconnect(m_flowScene, &QtNodes::FlowScene::nodeMoved, this, &ProjectBridge::onNodeMoved);
    disconnect(m_flowScene, &QtNodes::FlowScene::nodeResized, this, &ProjectBridge::onNodeResized);
    disconnect(m_flowScene, &QtNodes::FlowScene::connectionCreated, this, &ProjectBridge::onConnectionCreated);
    disconnect(m_flowScene, &QtNodes::FlowScene::connectionDeleted, this, &ProjectBridge::onConnectionDeleted);
    disconnect(m_flowScene, &QGraphicsScene::selectionChanged, this, &ProjectBridge::onSelectionChanged);
}

void babelwires::ProjectBridge::connectToFlowScene(QtNodes::FlowScene& flowScene) {
    assert(!m_flowScene && "The ProjectBridge can connect only once to a flow scene");
    m_flowScene = &flowScene;
    connect(m_flowScene, &QtNodes::FlowScene::nodeCreated, this, &ProjectBridge::onNodeCreated);
    connect(m_flowScene, &QtNodes::FlowScene::nodeDeleted, this, &ProjectBridge::onNodeDeleted);
    connect(m_flowScene, &QtNodes::FlowScene::nodeMoved, this, &ProjectBridge::onNodeMoved);
    connect(m_flowScene, &QtNodes::FlowScene::nodeResized, this, &ProjectBridge::onNodeResized);
    connect(m_flowScene, &QtNodes::FlowScene::connectionCreated, this, &ProjectBridge::onConnectionCreated);
    connect(m_flowScene, &QtNodes::FlowScene::connectionDeleted, this, &ProjectBridge::onConnectionDeleted);
    connect(m_flowScene, &QGraphicsScene::selectionChanged, this, &ProjectBridge::onSelectionChanged);
}

const babelwires::UiProjectContext& babelwires::ProjectBridge::getContext() const {
    return m_projectContext;
}

babelwires::MainWindow* babelwires::ProjectBridge::getMainWindow() const {
    return m_mainWindow;
}

void babelwires::ProjectBridge::setMainWindow(MainWindow* mainWindow) {
    m_mainWindow = mainWindow;
}

void babelwires::ProjectBridge::onNodeCreated(QtNodes::Node& n) {
    switch (m_state) {
        case State::WaitingForNewNode: {
            QtNodes::NodeDataModel& dataModel = *n.nodeDataModel();
            auto& elementNodeModel = dynamic_cast<ElementNodeModel&>(dataModel);
            const NodeId elementId = elementNodeModel.getNodeId();
            m_nodeFromNodeId.insert(std::make_pair(elementId, &n));
            m_state = State::ListeningToFlowScene;
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

void babelwires::ProjectBridge::onNodeDeleted(QtNodes::Node& n) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            QtNodes::NodeDataModel& dataModel = *n.nodeDataModel();
            auto& elementNodeModel = dynamic_cast<ElementNodeModel&>(dataModel);
            NodeId elementId = elementNodeModel.getNodeId();
            m_projectObserver.ignoreRemovedNode(elementId);
#ifndef NDEBUG
            auto numRemoved =
#endif
                m_nodeFromNodeId.erase(elementId);
            assert((numRemoved == 1) && "Expected the node to be in the scene");
            scheduleCommand(std::make_unique<RemoveNodeCommand>("Remove node", elementId));
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

void babelwires::ProjectBridge::onNodeMoved(QtNodes::Node& n, const QPointF& newLocation) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            // Work out if this actually moves the node to a different position.
            QtNodes::NodeDataModel& dataModel = *n.nodeDataModel();
            auto& elementNodeModel = dynamic_cast<ElementNodeModel&>(dataModel);
            const NodeId elementId = elementNodeModel.getNodeId();

            AccessModelScope scope(*this);
            const Node* element = scope.getProject().getNode(elementId);
            assert(element && "The element should already be in the project");
            const UiPosition& uiPosition = element->getNodeData().m_uiData.m_uiPosition;
            UiPosition newPosition{static_cast<UiCoord>(newLocation.x()), static_cast<UiCoord>(newLocation.y())};
            if (uiPosition != newPosition) {
                std::string commandName = "Move " + elementNodeModel.caption().toStdString();
                scheduleCommand(std::make_unique<MoveNodeCommand>(commandName, elementId, newPosition));
                m_projectObserver.ignoreMovedNode(elementId);
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

void babelwires::ProjectBridge::onNodeResized(QtNodes::Node& n, const QSize& newSize) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            // Work out if this actually moves the node to a different position.
            auto& elementNodeModel = dynamic_cast<ElementNodeModel&>(*n.nodeDataModel());
            const NodeId elementId = elementNodeModel.getNodeId();

            AccessModelScope scope(*this);
            const Node* element = scope.getProject().getNode(elementId);
            assert(element && "The element should already be in the project");
            const int currentWidth = element->getUiSize().m_width;
            const int newWidth = elementNodeModel.embeddedWidget()->size().width();
            if (newWidth != currentWidth) {
                std::string commandName = "Resize " + elementNodeModel.caption().toStdString();
                scheduleCommand(std::make_unique<ResizeNodeCommand>(commandName, elementId, UiSize{newWidth}));
                m_projectObserver.ignoreResizedNode(elementId);
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

namespace {
    babelwires::ElementNodeModel& getNodeData(QtNodes::Node* n) {
        return dynamic_cast<babelwires::ElementNodeModel&>(*n->nodeDataModel());
    }

    babelwires::NodeId getIdFromNode(QtNodes::Node* n) { return getNodeData(n).getNodeId(); }
} // namespace

QtNodes::Node* babelwires::ProjectBridge::getNodeFromId(NodeId id) {
    auto it = m_nodeFromNodeId.find(id);
    return (it != m_nodeFromNodeId.end()) ? it->second : nullptr;
}

void babelwires::ProjectBridge::onConnectionCreated(const QtNodes::Connection& c) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            assert((m_connectedConnections.find0(&c) == m_connectedConnections.end()) &&
                   "Cannot already know about this connection.");
            if (c.getNode(QtNodes::PortType::In) && c.getNode(QtNodes::PortType::Out)) {
                // Not sure how this could happen, but just in case.
                onConnectionConnected(c);
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
    // We are always interested in connection events, regardless of where they originate.
    connect(&c, &QtNodes::Connection::updated, this, &ProjectBridge::onConnectionUpdated);
}

void babelwires::ProjectBridge::onConnectionDeleted(const QtNodes::Connection& c) {
    disconnect(&c, &QtNodes::Connection::updated, this, &ProjectBridge::onConnectionUpdated);

    switch (m_state) {
        case State::ListeningToFlowScene: {
            if (m_connectedConnections.find0(&c) != m_connectedConnections.end()) {
                onConnectionDisconnected(c);
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

void babelwires::ProjectBridge::onConnectionUpdated(const QtNodes::Connection& c) {
    switch (m_state) {
        case State::ListeningToFlowScene: {
            bool isConnection = false;
            auto it = m_connectedConnections.find0(&c);
            if (it != m_connectedConnections.end()) {
                if (!it.getValue().isSameFlowSceneConnection(c)) {
                    if (c.getNode(QtNodes::PortType::In) && c.getNode(QtNodes::PortType::Out)) {
                        onConnectionAdjusted(c);
                    } else {
                        onConnectionDisconnected(c);
                    }
                }
            } else if (c.getNode(QtNodes::PortType::In) && c.getNode(QtNodes::PortType::Out)) {
                onConnectionConnected(c);
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

void babelwires::ProjectBridge::addNodeToFlowScene(const Node* featureElement) {
    const NodeId elementId = featureElement->getNodeId();
    auto newNodeData = std::make_unique<ElementNodeModel>(*this, elementId);
    QtNodes::Node& newNode = m_flowScene->createNode(std::move(newNodeData));
    // Place new nodes at the very top.
    newNode.nodeGraphicsObject().setZValue(std::numeric_limits<qreal>::max());
    if (m_newNodesShouldBeSelected) {
        newNode.nodeGraphicsObject().setSelected(true);
    }

    const UiPosition& uiPos = featureElement->getUiPosition();
    const QPointF newPos(uiPos.m_x, uiPos.m_y);
    m_flowScene->setNodePosition(newNode, newPos);

#ifndef NDEBUG
    auto resultPair =
#endif
        m_nodeFromNodeId.insert(std::make_pair(elementId, &newNode));
    assert(resultPair.second && "There's already a node for that elementId");
}

void babelwires::ProjectBridge::removeNodeFromFlowScene(NodeId elementId) {
    auto it = m_nodeFromNodeId.find(elementId);
    assert((it != m_nodeFromNodeId.end()) && "Trying to remove unrecognized node");
    QtNodes::Node* node = it->second;
#ifndef NDEBUG
    {
        auto elementNode = dynamic_cast<const babelwires::ElementNodeModel*>(node->nodeDataModel());
        assert(elementNode && "Unexpected node in graph");
        assert((elementNode->getNodeId() == elementId) && "Node to remove had unexpected id");
    }
#endif
    m_nodeFromNodeId.erase(it);
    m_flowScene->removeNode(*node);
}

void babelwires::ProjectBridge::moveNodeInFlowScene(NodeId elementId, const UiPosition& newPosition) {
    QtNodes::Node* node = getNodeFromId(elementId);
    const QPointF newPos(newPosition.m_x, newPosition.m_y);
    m_flowScene->setNodePosition(*node, newPos);
}

void babelwires::ProjectBridge::resizeNodeInFlowScene(NodeId elementId, const UiSize& newSize) {
    QtNodes::Node* node = getNodeFromId(elementId);
    babelwires::ElementNodeModel& elementNodeModel = getNodeData(node);
    elementNodeModel.setSize(newSize);
    node->nodeGraphicsObject().setGeometryChanged();
    node->nodeGeometry().recalculateSize();
    node->nodeGraphicsObject().update();
    node->nodeGraphicsObject().moveConnections();
}

void babelwires::ProjectBridge::onConnectionConnected(const QtNodes::Connection& c) {
    AccessModelScope scope(*this);
    FlowSceneConnectionInfo flowSceneInfo(c);
    ConnectionDescription modelInfo = describeConnection(scope, flowSceneInfo);
    scheduleCommand(modelInfo.getConnectionCommand());
    m_connectedConnections.insert_or_assign(&c, modelInfo, std::move(flowSceneInfo));
    m_projectObserver.ignoreAddedConnection(std::move(modelInfo));
}

void babelwires::ProjectBridge::onConnectionDisconnected(const QtNodes::Connection& c) {
    AccessModelScope scope(*this);

    auto it = m_connectedConnections.find0(&c);
    assert((it != m_connectedConnections.end()) && "Cannot disconnect a connection we don't know about");

    ConnectionDescription modelInfo(it.getKey1());
    scheduleCommand(modelInfo.getDisconnectionCommand());
    m_connectedConnections.erase(it);
    m_projectObserver.ignoreRemovedConnection(std::move(modelInfo));
}

void babelwires::ProjectBridge::onConnectionAdjusted(const QtNodes::Connection& c) {
    AccessModelScope scope(*this);
    auto command = std::make_unique<CompoundCommand<Project>>("Adjust connection");

    auto it = m_connectedConnections.find0(&c);
    assert((it != m_connectedConnections.end()) && "Cannot adjust a connection we don't know about");

    FlowSceneConnectionInfo flowSceneInfo = std::move(it.getValue());

    {
        ConnectionDescription modelInfo(it.getKey1());
        command->addSubCommand(modelInfo.getDisconnectionCommand());
        m_connectedConnections.erase(it);
        m_projectObserver.ignoreRemovedConnection(std::move(modelInfo));
    }

    // Update the map.
    {
        ConnectionDescription modelInfo = describeConnection(scope, flowSceneInfo);
        m_projectObserver.ignoreAddedConnection(modelInfo);
        command->addSubCommand(modelInfo.getConnectionCommand());
        m_connectedConnections.insert_or_assign(&c, std::move(modelInfo), std::move(flowSceneInfo));
    }

    scheduleCommand(std::move(command));
}

QWidget* babelwires::ProjectBridge::getFlowGraphWidget() {
    auto views = m_flowScene->views();
    assert(!views.isEmpty() && "There should be a flowView viewing the flowScene");
    return views[0];
}

void babelwires::ProjectBridge::addConnectionToFlowScene(const ConnectionDescription& connection) {
    QtNodes::Node* nodeIn = getNodeFromId(connection.m_targetId);
    assert(nodeIn &&
           "Ensure nodeIn is in the scene before calling this (or nodeIn is missing from m_nodeFromNodeId map)");
    QtNodes::Node* nodeOut = getNodeFromId(connection.m_sourceId);
    assert(nodeIn &&
           "Ensure nodeOut is in the scene before calling this (or nodeOut is missing from m_nodeFromNodeId map).");

    ElementNodeModel& nodeDataIn = getNodeData(nodeIn);
    ElementNodeModel& nodeDataOut = getNodeData(nodeOut);

    AccessModelScope scope(*this);
    QtNodes::PortIndex portIndexIn =
        nodeDataIn.getPortAtPath(scope, QtNodes::PortType::In, connection.m_targetPath);
    QtNodes::PortIndex portIndexOut =
        nodeDataOut.getPortAtPath(scope, QtNodes::PortType::Out, connection.m_sourcePath);

    std::shared_ptr<QtNodes::Connection> c =
        m_flowScene->createConnection(*nodeIn, portIndexIn, *nodeOut, portIndexOut);

    m_connectedConnections.insert_or_assign(c.get(), connection, FlowSceneConnectionInfo(*c));
}

void babelwires::ProjectBridge::removeConnectionFromFlowScene(const ConnectionDescription& connection) {
    auto it = m_connectedConnections.find1(connection);
    assert((it != m_connectedConnections.end()) && "Was expecting to know about this connection");
    m_flowScene->deleteConnection(*it.getKey0());
    m_connectedConnections.erase(it);
}

void babelwires::ProjectBridge::onNodeContentChanged(NodeId elementId) {
    QtNodes::Node* n = getNodeFromId(elementId);
    // Update node the visuals
    // TODO Inefficient.
    n->nodeGraphicsObject().update();
    // Trigger any other updates.
    getNodeData(n).getModel().valuesMayHaveChanged();
}

void babelwires::ProjectBridge::processAndHandleModelChanges() {
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

void babelwires::ProjectBridge::scheduleCommand(std::unique_ptr<Command<Project>> command) {
    if (m_scheduledCommand) {
        assert(m_scheduledCommand->shouldSubsume(*command, false) && "Commands scheduled together should subsume");
        m_scheduledCommand->subsume(std::move(command));
    } else {
        m_scheduledCommand = std::move(command);
        // Schedule the execution of the command.
        QTimer::singleShot(0, this, &ProjectBridge::onIdle);
    }
}

void babelwires::ProjectBridge::onIdle() {
    if (m_scheduledCommand) {
        ModifyModelScope scope(*this);
        std::unique_ptr<Command<Project>> scheduledCommand = std::move(m_scheduledCommand);
        scope.getCommandManager().executeAndStealCommand(scheduledCommand);
    }
}

bool babelwires::ProjectBridge::executeCommandSynchronously(std::unique_ptr<Command<Project>> command) {
    ModifyModelScope scope(*this);
    std::unique_ptr<Command<Project>> commandPtr = std::move(command);
    return scope.getCommandManager().executeAndStealCommand(commandPtr);
}

bool babelwires::ProjectBridge::executeAddNodeCommand(std::unique_ptr<AddNodeCommand> command) {
    ModifyModelScope scope(*this);
    AddNodeCommand& addNodeCommand = *command;
    std::unique_ptr<Command<Project>> commandPtr = std::move(command);
    if (!scope.getCommandManager().executeAndStealCommand(commandPtr)) {
        return false;
    }

    NodeId newNodeId = addNodeCommand.getNodeId();
    m_projectObserver.ignoreAddedNode(newNodeId);
    m_state = State::WaitingForNewNode;
    return true;
}

babelwires::ConnectionDescription babelwires::ProjectBridge::describeConnection(babelwires::AccessModelScope& scope,
                                                                                const FlowSceneConnectionInfo& c) {
    ConnectionDescription connectionDescription;
    {
        const QtNodes::Node* const node = c.m_inNode;
        QtNodes::PortIndex portIndex = c.m_inPort;
        auto elementNode = dynamic_cast<const babelwires::ElementNodeModel*>(node->nodeDataModel());
        assert(elementNode && "Unexpected node in graph");
        connectionDescription.m_targetId = elementNode->getNodeId();
        connectionDescription.m_targetPath =
            elementNode->getPathAtPort(scope, QtNodes::PortType::In, portIndex);
    }
    {
        const QtNodes::Node* const node = c.m_outNode;
        QtNodes::PortIndex portIndex = c.m_outPort;
        auto elementNode = dynamic_cast<const babelwires::ElementNodeModel*>(node->nodeDataModel());
        assert(elementNode && "Unexpected node in graph");
        connectionDescription.m_sourceId = elementNode->getNodeId();
        connectionDescription.m_sourcePath =
            elementNode->getPathAtPort(scope, QtNodes::PortType::Out, portIndex);
    }
    return connectionDescription;
}

babelwires::ProjectData babelwires::ProjectBridge::getDataFromSelectedNodes() {
    std::vector<QtNodes::Node*> selectedNodes = m_flowScene->selectedNodes();
    ProjectData projectData;
    AccessModelScope scope(*this);
    const Project& project = scope.getProject();
    projectData.m_projectId = project.getProjectId();
    for (const auto& node : selectedNodes) {
        QtNodes::NodeDataModel& dataModel = *node->nodeDataModel();
        auto& elementNodeModel = dynamic_cast<ElementNodeModel&>(dataModel);
        const NodeId elementId = elementNodeModel.getNodeId();

        const Node* const element = project.getNode(elementId);
        assert(element && "The node is not in the project");
        projectData.m_nodes.emplace_back(element->extractNodeData());
    }
    return projectData;
}

void babelwires::ProjectBridge::selectNewNodes() {
    m_flowScene->clearSelection();
    m_newNodesShouldBeSelected = true;
}

void babelwires::ProjectBridge::onSelectionChanged() {
    emit nodeSelectionChanged(static_cast<int>(m_flowScene->selectedNodes().size()));
}
