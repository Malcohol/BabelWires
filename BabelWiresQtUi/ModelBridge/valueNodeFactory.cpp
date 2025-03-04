/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/valueNodeFactory.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeNodeModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>

#include <QtWidgets/QFileDialog>
#include <nodes/FlowScene>

babelwires::ValueNodeFactory::ValueNodeFactory(ProjectBridge* projectBridge, TypeRef typeOfValue)
    : NodeFactoryBase(projectBridge)
    , m_typeOfValue(typeOfValue) {}

QString babelwires::ValueNodeFactory::name() const {
    return m_typeOfValue.toString().c_str();
}

std::unique_ptr<QtNodes::NodeDataModel> babelwires::ValueNodeFactory::createNode() const {
    auto newDataPtr = std::make_unique<ValueNodeData>(m_typeOfValue);

    auto commandPtr = std::make_unique<AddNodeCommand>("Add Value Node", std::move(newDataPtr));
    AddNodeCommand& addNodeCommand = *commandPtr;
    if (m_projectBridge->executeAddNodeCommand(std::move(commandPtr))) {
        return std::make_unique<NodeNodeModel>(*m_projectBridge, addNodeCommand.getNodeId());
    }
    return nullptr;
}
