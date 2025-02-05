/**
 * NodeNodeModel is the NodeDataModels corresponding to Nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/nodeNodeModel.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/nodeContentsView.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/ModelBridge/rowModelDelegate.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <cassert>

#include <QMenu>

babelwires::NodeNodeModel::NodeNodeModel(ProjectBridge& projectBridge, NodeId elementId)
    : BaseNodeModel(projectBridge)
    , m_nodeId(elementId)
    , m_view(new NodeContentsView(elementId, projectBridge)) {
    auto delegate = new RowModelDelegate(this, projectBridge);
    m_view->setItemDelegate(delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customContextMenuRequested(QPoint)));

    setContents("Todo", elementId);

    connect(m_view, SIGNAL(clicked(const QModelIndex&)), m_model, SLOT(onClicked(const QModelIndex&)));
}

babelwires::NodeNodeModel::~NodeNodeModel() {}

void babelwires::NodeNodeModel::setContents(std::string label, NodeId elementId) {
    AccessModelScope scope(m_projectBridge);
    const Node* node = scope.getProject().getNode(elementId);
    assert(node && "The ID must correspond to an node in the project");
    m_model = new NodeContentsModel(m_view, elementId, m_projectBridge);
    m_view->setModel(m_model);
}

babelwires::NodeId babelwires::NodeNodeModel::getNodeId() const {
    return m_nodeId;
}

QWidget* babelwires::NodeNodeModel::embeddedWidget() {
    return m_view;
}

unsigned int babelwires::NodeNodeModel::nPorts(QtNodes::PortType portType) const {
    AccessModelScope scope(m_projectBridge);
    return m_model->getNumRows(scope);
}

QtNodes::NodeDataType babelwires::NodeNodeModel::dataType(QtNodes::PortType portType,
                                                          QtNodes::PortIndex portIndex) const {
    AccessModelScope scope(m_projectBridge);
    if (portType == QtNodes::PortType::In) {
        return getDataTypeFromTreeValueNode(getInput(scope, portIndex));
    } else {
        return getDataTypeFromTreeValueNode(getOutput(scope, portIndex));
    }
}

const babelwires::ValueTreeNode* babelwires::NodeNodeModel::getInput(AccessModelScope& scope, int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        return entry->getInput();
    } else {
        return nullptr;
    }
}

const babelwires::ValueTreeNode* babelwires::NodeNodeModel::getOutput(AccessModelScope& scope, int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        return entry->getOutput();
    } else {
        return nullptr;
    }
}

QtNodes::NodeDataType babelwires::NodeNodeModel::getDataTypeFromTreeValueNode(const babelwires::ValueTreeNode* f) {
    if (f && f->getKind() != "") {
        return QtNodes::NodeDataType{f->getKind().c_str(), ""};
    }
    return QtNodes::NodeDataType();
}

const babelwires::Path& babelwires::NodeNodeModel::getPathAtPort(AccessModelScope& scope, QtNodes::PortType portType,
                                                                 QtNodes::PortIndex portIndex) const {
    const ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex);
    assert(entry && "Check before calling this.");

    return entry->getPath();
}

QtNodes::PortIndex babelwires::NodeNodeModel::getPortAtPath(AccessModelScope& scope, QtNodes::PortType portType,
                                                            const Path& path) const {
    const Node* node = m_model->getNode(scope);
    assert(node && "Check before calling this.");
    const int row = node->getContentsCache().getIndexOfPath((portType == QtNodes::PortType::In), path);
    assert((row != -1) && "Path did not lead to a known feature");
    // TODO Assert that the entry corresponds to the correct portType.
    return row;
}

QString babelwires::NodeNodeModel::caption() const {
    AccessModelScope scope(m_projectBridge);
    if (const Node* node = scope.getProject().getNode(m_nodeId)) {
        return QString(node->getLabel().c_str());
    } else {
        return "Dying node";
    }
}

void babelwires::NodeNodeModel::customContextMenuRequested(const QPoint& pos) {
    QModelIndex index = m_view->indexAt(pos);
    QMenu* const menu = m_model->getContextMenu(index);
    if (menu) {
        menu->popup(m_view->mapToGlobalCorrect(pos));
    }
}

void babelwires::NodeNodeModel::setSize(const UiSize& newSize) {
    QWidget* widget = embeddedWidget();
    widget->setMinimumWidth(newSize.m_width);
    widget->setMaximumWidth(newSize.m_width);
}

babelwires::NodeContentsModel& babelwires::NodeNodeModel::getModel() {
    assert(m_model && "Model should always exist");
    return *m_model;
}
