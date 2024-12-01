/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/valueNodeFactory.hpp>

#include <BabelWiresQtUi/ModelBridge/elementNodeModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/addElementCommand.hpp>
#include <BabelWiresLib/Project/Nodes/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileElement/targetFileElementData.hpp>

#include <QtWidgets/QFileDialog>
#include <nodes/FlowScene>

babelwires::ValueNodeFactory::ValueNodeFactory(ProjectBridge* projectBridge, TypeRef typeOfValue)
    : NodeFactoryBase(projectBridge)
    , m_typeOfValue(typeOfValue) {}

QString babelwires::ValueNodeFactory::name() const {
    return m_typeOfValue.toString().c_str();
}

std::unique_ptr<QtNodes::NodeDataModel> babelwires::ValueNodeFactory::createNode() const {
    auto newDataPtr = std::make_unique<ValueElementData>(m_typeOfValue);

    auto commandPtr = std::make_unique<AddElementCommand>("Add Value Element", std::move(newDataPtr));
    AddElementCommand& addElementCommand = *commandPtr;
    if (m_projectBridge->executeAddElementCommand(std::move(commandPtr))) {
        return std::make_unique<ElementNodeModel>(*m_projectBridge, addElementCommand.getElementId());
    }
    return nullptr;
}
