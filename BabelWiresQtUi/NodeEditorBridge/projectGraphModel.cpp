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
        [this](NodeId nodeId, const UiPosition& uiPosition) { moveNodeInFlowScene(nodeId, uiPosition); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_nodeWasResized.subscribe(
        [this](NodeId nodeId, const UiSize& newSize) { resizeNodeInFlowScene(nodeId, newSize); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasAdded.subscribe(
        [this](const ConnectionDescription& connection) { addConnectionToFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(m_projectObserver.m_connectionWasRemoved.subscribe(
        [this](const ConnectionDescription& connection) { removeConnectionFromFlowScene(connection); }));

    m_projectObserverSubscriptions.emplace_back(
        m_projectObserver.m_contentWasChanged.subscribe([this](NodeId nodeId) { onNodeContentChanged(nodeId); }));
}

std::unordered_set<NodeId> babelwires::ProjectGraphModel::allNodeIds() const {};

std::unordered_set<ConnectionId> babelwires::ProjectGraphModel::allConnectionIds(QtNodes::NodeId const nodeId) const {};

std::unordered_set<ConnectionId> babelwires::ProjectGraphModel::connections(QtNodes::NodeId nodeId, PortType portType,
                                                                            PortIndex index) const {};

bool babelwires::ProjectGraphModel::connectionExists(ConnectionId const connectionId) const {};

bool babelwires::ProjectGraphModel::connectionPossible(ConnectionId const connectionId) const {};

void babelwires::ProjectGraphModel::addConnection(ConnectionId const connectionId) {};

bool babelwires::ProjectGraphModel::nodeExists(QtNodes::NodeId const nodeId) const {};

QVariant babelwires::ProjectGraphModel::nodeData(QtNodes::NodeId nodeId, NodeRole role) const {
    // TODO: I don't think this look up should be required for anything other than Widget.
    const auto it = m_nodeModels.find(nodeId);
    assert(it != m_nodeModels.end());
    const NodeNodeModel& nodeModel = *it.second;
    switch (role) {
        case NodeRole::Type:
            // TODO This should maybe be something like processor / value / source / target.
            return "TODO type";
        case NodeRole::Position:
            {
                AccessModelScope scope(*this);
                const Node *const node = scope.getProject().getNode(nodeId);
                const UiPosition position = node.getUiPosition();
                return QPointF{position.m_x, position.m_y};
            }
        case NodeRole::Size:
            {
                AccessModelScope scope(*this);
                const Node *const node = scope.getProject().getNode(nodeId);
                const UiSize size = node.getUiSize();
                return QSize{size.m_width, nodeModel.embeddedWidget()->height() };
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

bool babelwires::ProjectGraphModel::deleteConnection(ConnectionId const connectionId) {};

bool babelwires::ProjectGraphModel::deleteNode(NodeId const nodeId) {};