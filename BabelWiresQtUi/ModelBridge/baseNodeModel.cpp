/**
 * BaseNodeModel is the base class shared by all the NodeDataModels in BabelWires.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/baseNodeModel.hpp"

babelwires::BaseNodeModel::BaseNodeModel(ProjectBridge& projectBridge)
    : m_projectBridge(projectBridge) {}

QString babelwires::BaseNodeModel::name() const {
    return nullptr;
}

QString babelwires::BaseNodeModel::caption() const {
    return nullptr;
}

unsigned int babelwires::BaseNodeModel::nPorts(QtNodes::PortType portType) const {
    return 0;
}

QtNodes::NodeDataType babelwires::BaseNodeModel::dataType(QtNodes::PortType portType,
                                                          QtNodes::PortIndex portIndex) const {
    return QtNodes::NodeDataType();
}

void babelwires::BaseNodeModel::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) {}

std::shared_ptr<QtNodes::NodeData> babelwires::BaseNodeModel::outData(QtNodes::PortIndex port) {
    return nullptr;
}

QWidget* babelwires::BaseNodeModel::embeddedWidget() {
    return nullptr;
}
