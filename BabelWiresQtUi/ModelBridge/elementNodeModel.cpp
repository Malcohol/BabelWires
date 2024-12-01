/**
 * ElementNodeModel is the NodeDataModels corresponding to Nodes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/elementNodeModel.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>
#include <BabelWiresQtUi/ModelBridge/featureModelDelegate.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>

#include <cassert>

#include <QMenu>

babelwires::ElementNodeModel::ElementNodeModel(ProjectBridge& projectBridge, NodeId elementId)
    : BaseNodeModel(projectBridge)
    , m_nodeId(elementId)
    , m_view(new FeatureView(elementId, projectBridge)) {
    auto delegate = new FeatureModelDelegate(this, projectBridge);
    m_view->setItemDelegate(delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customContextMenuRequested(QPoint)));

    setContents("Todo", elementId);

    connect(m_view, SIGNAL(clicked(const QModelIndex&)), m_model, SLOT(onClicked(const QModelIndex&)));
}

babelwires::ElementNodeModel::~ElementNodeModel() {}

void babelwires::ElementNodeModel::setContents(std::string label, NodeId elementId) {
    AccessModelScope scope(m_projectBridge);
    const Node* node = scope.getProject().getNode(elementId);
    assert(node && "The ID must correspond to an node in the project");
    m_model = new FeatureModel(m_view, elementId, m_projectBridge);
    m_view->setModel(m_model);
}

babelwires::NodeId babelwires::ElementNodeModel::getNodeId() const {
    return m_nodeId;
}

QWidget* babelwires::ElementNodeModel::embeddedWidget() {
    return m_view;
}

unsigned int babelwires::ElementNodeModel::nPorts(QtNodes::PortType portType) const {
    AccessModelScope scope(m_projectBridge);
    return m_model->getNumRows(scope);
}

QtNodes::NodeDataType babelwires::ElementNodeModel::dataType(QtNodes::PortType portType,
                                                             QtNodes::PortIndex portIndex) const {
    AccessModelScope scope(m_projectBridge);
    if (portType == QtNodes::PortType::In) {
        return getDataTypeFromFeature(getInput(scope, portIndex));
    } else {
        return getDataTypeFromFeature(getOutput(scope, portIndex));
    }
}

const babelwires::ValueTreeNode* babelwires::ElementNodeModel::getInput(AccessModelScope& scope, int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        return entry->getInput();
    } else {
        return nullptr;
    }
}

const babelwires::ValueTreeNode* babelwires::ElementNodeModel::getOutput(AccessModelScope& scope,
                                                                          int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        return entry->getOutput();
    } else {
        return nullptr;
    }
}

QtNodes::NodeDataType babelwires::ElementNodeModel::getDataTypeFromFeature(const babelwires::ValueTreeNode* f) {
    if (f && f->getKind() != "") {
        return QtNodes::NodeDataType{f->getKind().c_str(), ""};
    }
    return QtNodes::NodeDataType();
}

const babelwires::Path& babelwires::ElementNodeModel::getPathAtPort(AccessModelScope& scope,
                                                                           QtNodes::PortType portType,
                                                                           QtNodes::PortIndex portIndex) const {
    const ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex);
    assert(entry && "Check before calling this.");

    return entry->getPath();
}

QtNodes::PortIndex babelwires::ElementNodeModel::getPortAtPath(AccessModelScope& scope, QtNodes::PortType portType,
                                                               const Path& path) const {
    const Node* node = m_model->getNode(scope);
    assert(node && "Check before calling this.");
    const int row = node->getContentsCache().getIndexOfPath((portType == QtNodes::PortType::In), path);
    assert((row != -1) && "Path did not lead to a known feature");
    // TODO Assert that the entry corresponds to the correct portType.
    return row;
}

QString babelwires::ElementNodeModel::caption() const {
    AccessModelScope scope(m_projectBridge);
    if (const Node* node = scope.getProject().getNode(m_nodeId)) {
        return QString(node->getLabel().c_str());
    } else {
        return "Dying node";
    }
}

void babelwires::ElementNodeModel::customContextMenuRequested(const QPoint& pos) {
    QModelIndex index = m_view->indexAt(pos);
    QMenu* const menu = m_model->getContextMenu(index);
    if (menu) {
        menu->popup(m_view->mapToGlobal(pos));
    }
}

void babelwires::ElementNodeModel::setSize(const UiSize& newSize) {
    QWidget* widget = embeddedWidget();
    widget->setMinimumWidth(newSize.m_width);
    widget->setMaximumWidth(newSize.m_width);
}

babelwires::FeatureModel& babelwires::ElementNodeModel::getModel() {
    assert(m_model && "Model should always exist");
    return *m_model;
}
