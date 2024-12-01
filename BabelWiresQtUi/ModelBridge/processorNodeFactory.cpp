/**
 * The factory which creates nodes for processors.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/processorNodeFactory.hpp>

#include <BabelWiresQtUi/ModelBridge/elementNodeModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/addElementCommand.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorElement/processorElementData.hpp>

#include <QtWidgets/QFileDialog>
#include <nodes/FlowScene>

babelwires::ProcessorNodeFactory::ProcessorNodeFactory(ProjectBridge* projectBridge,
                                                       const ProcessorFactory* processorFactory)
    : NodeFactoryBase(projectBridge)
    , m_processorFactory(processorFactory) {}

QString babelwires::ProcessorNodeFactory::name() const {
    return m_processorFactory->getName().c_str();
}

std::unique_ptr<QtNodes::NodeDataModel> babelwires::ProcessorNodeFactory::createNode() const {
    auto newDataPtr = std::make_unique<ProcessorElementData>();
    newDataPtr->m_factoryIdentifier = m_processorFactory->getIdentifier();
    newDataPtr->m_factoryVersion = m_processorFactory->getVersion();

    auto commandPtr = std::make_unique<AddElementCommand>("Add processor", std::move(newDataPtr));
    AddElementCommand& addElementCommand = *commandPtr;
    if (m_projectBridge->executeAddElementCommand(std::move(commandPtr))) {
        return std::make_unique<ElementNodeModel>(*m_projectBridge, addElementCommand.getElementId());
    }
    return nullptr;
}
