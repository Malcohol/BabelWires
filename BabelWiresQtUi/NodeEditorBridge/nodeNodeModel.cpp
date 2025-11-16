/**
 * NodeNodeModel is the NodeDataModels corresponding to Nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/nodeNodeModel.hpp>

#include <BabelWiresQtUi/ContextMenu/contextMenu.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsView.hpp>
#include <BabelWiresQtUi/ModelBridge/rowModelDelegate.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <cassert>

#include <QMenu>

babelwires::NodeNodeModel::NodeNodeModel(ProjectGraphModel& projectGraphModel, NodeId nodeId, const Node& node)
    : m_nodeId(nodeId)
    , m_view(new NodeContentsView(nodeId, projectGraphModel)) {
    auto delegate = new RowModelDelegate(this, projectGraphModel);
    m_view->setItemDelegate(delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customContextMenuRequested(QPoint)));
    m_model = new NodeContentsModel(m_view, nodeId, projectGraphModel);
    m_view->setModel(m_model);
    connect(m_view, SIGNAL(clicked(const QModelIndex&)), m_model, SLOT(onClicked(const QModelIndex&)));
    m_cachedPosition =
        QPointF{static_cast<qreal>(node.getUiPosition().m_x), static_cast<qreal>(node.getUiPosition().m_y)};
}

babelwires::NodeNodeModel::~NodeNodeModel() = default;

const QWidget* babelwires::NodeNodeModel::getEmbeddedWidget() const {
    return m_view;
}

QWidget* babelwires::NodeNodeModel::getEmbeddedWidget() {
    return m_view;
}

unsigned int babelwires::NodeNodeModel::getNumberOfPorts(const AccessModelScope& scope,
                                                         QtNodes::PortType portType) const {
    return m_model->getNumRows(scope);
}

QtNodes::NodeDataType babelwires::NodeNodeModel::getDataType(const AccessModelScope& scope, QtNodes::PortType portType,
                                                             QtNodes::PortIndex portIndex) const {
    const auto [valueTreeNode, hasUnassignedTypeVariable] = (portType == QtNodes::PortType::In)
        ? getInputInfo(scope, portIndex)
        : getOutputInfo(scope, portIndex);
    return getDataTypeFromTreeValueNode(valueTreeNode, hasUnassignedTypeVariable);
}

const babelwires::Type* babelwires::NodeNodeModel::getInputType(const AccessModelScope& scope,
                                                                QtNodes::PortIndex portIndex) const {
    if (const ValueTreeNode* const input = std::get<0>(getInputInfo(scope, portIndex))) {
        return &input->getType();
    }
    return nullptr;
}

const babelwires::Type* babelwires::NodeNodeModel::getOutputType(const AccessModelScope& scope,
                                                                 QtNodes::PortIndex portIndex) const {
    if (const ValueTreeNode* const output = std::get<0>(getOutputInfo(scope, portIndex))) {
        return &output->getType();
    }
    return nullptr;
}

std::tuple<const babelwires::ValueTreeNode*, bool> babelwires::NodeNodeModel::getInputInfo(const AccessModelScope& scope,
                                                                     int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        // We don't allow connections to compounds containing an unassigned type variable.
        // However, a type variable itself can be the input to a connection, since we can
        // use the type of the input as the assignment to the type variable.
        return {entry->getInput(), entry->hasUnassignedInputTypeVariable()};
    } else {
        return {nullptr, false};
    }
}

std::tuple<const babelwires::ValueTreeNode*, bool> babelwires::NodeNodeModel::getOutputInfo(const AccessModelScope& scope,
                                                                      int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        // We don't allow connections from compounds containing an unassigned type variable, or
        // type variables themselves.
        // TODO Consider whether we should use connections from type variables to determine their type assignment.
        return {entry->getOutput(), entry->isOrHasUnassignedOutputTypeVariable()};
    } else {
        return {nullptr, false};
    }
}

QtNodes::NodeDataType babelwires::NodeNodeModel::getDataTypeFromTreeValueNode(const babelwires::ValueTreeNode* valueTreeNode, bool hasUnassignedTypeVariable) {
    if (valueTreeNode && !hasUnassignedTypeVariable && (valueTreeNode->getFlavour() != "")) {
        return QtNodes::NodeDataType{valueTreeNode->getFlavour().c_str(), ""};
    }
    return QtNodes::NodeDataType();
}

const babelwires::Path& babelwires::NodeNodeModel::getPathAtPort(const AccessModelScope& scope,
                                                                 QtNodes::PortType portType,
                                                                 QtNodes::PortIndex portIndex) const {
    const ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex);
    assert(entry && "Check before calling this.");

    return entry->getPath();
}

QtNodes::PortIndex babelwires::NodeNodeModel::getPortAtPath(const AccessModelScope& scope, QtNodes::PortType portType,
                                                            const Path& path) const {
    const Node* const node = m_model->getNode(scope);
    assert(node && "Check before calling this.");
    const int row = node->getContentsCache().getIndexOfPath((portType == QtNodes::PortType::In), path);
    assert((row != -1) && "Path did not lead to a ValueTreeNode");
    // TODO Assert that the entry corresponds to the correct portType.
    return row;
}

QString babelwires::NodeNodeModel::getCaption(const AccessModelScope& scope) const {
    if (const Node* const node = scope.getProject().getNode(m_nodeId)) {
        std::string label = node->getLabel();
        constexpr unsigned int maxLabelLength = 35;
        if (label.size() > maxLabelLength) {
            // TODO Non-unicode truncation.
            label = label.substr(0, maxLabelLength - 1) + "\u2026";
        }
        return QString(label.c_str());
    } else {
        return "Dying node";
    }
}

void babelwires::NodeNodeModel::customContextMenuRequested(const QPoint& pos) {
    QModelIndex index = m_view->indexAt(pos);
    std::vector<ContextMenuEntry> entries;
    m_model->getContextMenuActions(entries, index);
    if (!entries.empty()) {
        ContextMenu* menu = new ContextMenu(*m_model, index);
        for (auto&& entry : entries) {
            menu->addContextMenuEntry(std::move(entry));
        }
        menu->popup(m_view->mapToGlobalCorrect(pos));
    }
}

QPointF babelwires::NodeNodeModel::getCachedPosition() const {
    return m_cachedPosition;
}

void babelwires::NodeNodeModel::setCachedPosition(QPointF newPos) {
    m_cachedPosition = newPos;
}

QSize babelwires::NodeNodeModel::getCachedSize() const {
    return m_cachedSize;
}

void babelwires::NodeNodeModel::setCachedSize(QSize newSize) {
    m_cachedSize = newSize;
}

void babelwires::NodeNodeModel::addInConnection(const QtNodes::ConnectionId& connectionId,
                                                const ConnectionDescription& connectionDescription) {
    assert(!isInConnection(connectionId));
    m_inConnections.emplace_back(std::tuple{connectionId, connectionDescription});
}

void babelwires::NodeNodeModel::addOutConnection(const QtNodes::ConnectionId& connectionId,
                                                 const ConnectionDescription& connectionDescription) {
    assert(!isOutConnection(connectionId));
    m_outConnections.emplace_back(std::tuple{connectionId, connectionDescription});
}

QtNodes::ConnectionId
babelwires::NodeNodeModel::removeInConnection(const ConnectionDescription& connectionDescription) {
    const auto it =
        std::find_if(m_inConnections.begin(), m_inConnections.end(), [connectionDescription](const auto& tuple) {
            return std::get<1>(tuple) == connectionDescription;
        });
    assert(it != m_inConnections.end());
    const QtNodes::ConnectionId connectionId = std::get<0>(*it);
    m_inConnections.erase(it);
    return connectionId;
}

QtNodes::ConnectionId
babelwires::NodeNodeModel::removeOutConnection(const ConnectionDescription& connectionDescription) {
    const auto it =
        std::find_if(m_outConnections.begin(), m_outConnections.end(), [connectionDescription](const auto& tuple) {
            return std::get<1>(tuple) == connectionDescription;
        });
    assert(it != m_outConnections.end());
    const QtNodes::ConnectionId connectionId = std::get<0>(*it);
    m_outConnections.erase(it);
    return connectionId;
}

std::unordered_set<QtNodes::ConnectionId> babelwires::NodeNodeModel::getAllConnectionIds() const {
    std::unordered_set<QtNodes::ConnectionId> connections;
    connections.reserve(m_inConnections.size() + m_outConnections.size());
    for (const auto& inConnection : m_inConnections) {
        connections.insert(std::get<0>(inConnection));
    }
    for (const auto& outConnection : m_outConnections) {
        connections.insert(std::get<0>(outConnection));
    }
    return connections;
}

std::unordered_set<QtNodes::ConnectionId> babelwires::NodeNodeModel::getConnections(QtNodes::PortType portType,
                                                                                    QtNodes::PortIndex index) const {
    std::unordered_set<QtNodes::ConnectionId> connectionsAtPort;
    if (portType == QtNodes::PortType::In) {
        for (const auto& inConnection : m_inConnections) {
            const auto& connectionId = std::get<0>(inConnection);
            if (connectionId.inPortIndex == index) {
                connectionsAtPort.insert(connectionId);
            }
        }
    } else {
        for (const auto& outConnection : m_outConnections) {
            const auto& connectionId = std::get<0>(outConnection);
            if (connectionId.outPortIndex == index) {
                connectionsAtPort.insert(connectionId);
            }
        }
    }
    return connectionsAtPort;
};

bool babelwires::NodeNodeModel::isInConnection(const QtNodes::ConnectionId& connectionId) const {
    assert(connectionId.inNodeId == m_nodeId);
    for (const auto& inConnection : m_inConnections) {
        const auto& inConnectionId = std::get<0>(inConnection);
        if ((inConnectionId.inPortIndex == connectionId.inPortIndex) &&
            (inConnectionId.outNodeId == connectionId.outNodeId) &&
            (inConnectionId.outPortIndex == connectionId.outPortIndex)) {
            return true;
        }
    }
    return false;
}

bool babelwires::NodeNodeModel::isOutConnection(const QtNodes::ConnectionId& connectionId) const {
    assert(connectionId.outNodeId == m_nodeId);
    for (const auto& outConnection : m_outConnections) {
        const auto& outConnectionId = std::get<0>(outConnection);
        if ((outConnectionId.outPortIndex == connectionId.outPortIndex) &&
            (outConnectionId.inNodeId == connectionId.inNodeId) &&
            (outConnectionId.inPortIndex == connectionId.inPortIndex)) {
            return true;
        }
    }
    return false;
}
