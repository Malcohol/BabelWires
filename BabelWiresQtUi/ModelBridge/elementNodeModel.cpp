/**
 * ElementNodeModel is the NodeDataModels corresponding to FeatureElements.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/elementNodeModel.hpp"
#include "BabelWires/Features/Path/featurePath.hpp"
#include "BabelWires/Features/numericFeature.hpp"
#include "BabelWires/Features/recordFeature.hpp"
#include "BabelWires/Features/stringFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/project.hpp"
#include "BabelWires/Project/uiPosition.hpp"
#include "BabelWiresQtUi/ModelBridge/accessModelScope.hpp"
#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"

#include <cassert>

#include <QMenu>

babelwires::ElementNodeModel::ElementNodeModel(ProjectBridge& projectBridge, ElementId elementId)
    : BaseNodeModel(projectBridge)
    , m_elementId(elementId)
    , m_view(new FeatureView(elementId, projectBridge)) {
    auto delegate = new FeatureModelDelegate(this, projectBridge);
    m_view->setItemDelegate(delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customContextMenuRequested(QPoint)));

    setContents("Todo", elementId);

    connect(m_view, SIGNAL(clicked(const QModelIndex&)), m_model, SLOT(onClicked(const QModelIndex&)));
}

babelwires::ElementNodeModel::~ElementNodeModel() {}

void babelwires::ElementNodeModel::setContents(std::string label, ElementId elementId) {
    AccessModelScope scope(m_projectBridge);
    const FeatureElement* element = scope.getProject().getFeatureElement(elementId);
    assert(element && "The ID must correspond to an element in the project");
    m_model = new FeatureModel(m_view, elementId, m_projectBridge);
    m_view->setModel(m_model);
}

babelwires::ElementId babelwires::ElementNodeModel::getElementId() const {
    return m_elementId;
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
        return getDataTypeFromFeature(getInputFeature(scope, portIndex));
    } else {
        return getDataTypeFromFeature(getOutputFeature(scope, portIndex));
    }
}

const babelwires::Feature* babelwires::ElementNodeModel::getInputFeature(AccessModelScope& scope, int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        return entry->getInputFeature();
    } else {
        return nullptr;
    }
}

const babelwires::Feature* babelwires::ElementNodeModel::getOutputFeature(AccessModelScope& scope,
                                                                          int portIndex) const {
    if (const babelwires::ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex)) {
        return entry->getOutputFeature();
    } else {
        return nullptr;
    }
}

QtNodes::NodeDataType babelwires::ElementNodeModel::getDataTypeFromFeature(const babelwires::Feature* f) {
    if (auto v = dynamic_cast<const ValueFeature*>(f)) {
        return QtNodes::NodeDataType{v->getValueType().c_str(), ""};
    }
    return QtNodes::NodeDataType();
}

const babelwires::FeaturePath& babelwires::ElementNodeModel::getPathAtPort(AccessModelScope& scope,
                                                                           QtNodes::PortType portType,
                                                                           QtNodes::PortIndex portIndex) const {
    const ContentsCacheEntry* entry = m_model->getEntry(scope, portIndex);
    assert(entry && "Check before calling this.");

    return entry->getPath();
}

QtNodes::PortIndex babelwires::ElementNodeModel::getPortAtPath(AccessModelScope& scope, QtNodes::PortType portType,
                                                               const FeaturePath& path) const {
    const FeatureElement* element = m_model->getFeatureElement(scope);
    assert(element && "Check before calling this.");
    const int row = element->getContentsCache().getIndexOfPath((portType == QtNodes::PortType::In), path);
    assert((row != -1) && "Path did not lead to a known feature");
    // TODO Assert that the entry corresponds to the correct portType.
    return row;
}

QString babelwires::ElementNodeModel::caption() const {
    AccessModelScope scope(m_projectBridge);
    if (const FeatureElement* element = scope.getProject().getFeatureElement(m_elementId)) {
        return QString(element->getLabel().c_str());
    } else {
        return "Dying node";
    }
}

void babelwires::ElementNodeModel::customContextMenuRequested(const QPoint& pos) {
    QModelIndex index = m_view->indexAt(pos);
    QMenu* const menu = m_model->getContextMenu(index);
    if (menu) {
        QWidget* const sceneWidget = m_projectBridge.getFlowGraphWidget();
        // We make the scene the parent of the menu. Otherwise it gets clipped within the node.
        menu->setParent(sceneWidget);
        // There is probably a more correct way of doing this.
        const QPoint globalPos = m_view->mapToGlobal(pos);
        const QPoint scenePos = sceneWidget->mapFromGlobal(globalPos);
        menu->popup(scenePos);
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
