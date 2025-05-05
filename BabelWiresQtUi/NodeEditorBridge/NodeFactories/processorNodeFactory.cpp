/**
 * The factory which creates nodes for processors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/processorNodeFactory.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>

babelwires::ProcessorNodeFactory::ProcessorNodeFactory(const ProcessorFactoryRegistry& processorFactoryRegistry)
    : m_processorFactoryRegistry(processorFactoryRegistry) {}

QString babelwires::ProcessorNodeFactory::getCategoryName() const {
    return m_processorFactoryRegistry.getRegistryName().c_str();
}

QList<QString> babelwires::ProcessorNodeFactory::getFactoryNames() const {
    // TODO
}

void babelwires::ProcessorNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName,
                                                  QPointF const scenePos, QWidget* parentForDialogs) {
    const ProcessorFactory* processorFactory = m_processorFactoryRegistry.getEntryByName(factoryName);
    assert(processorFactory);

    auto newNodeData = std::make_unique<ProcessorNodeData>();
    newNodeData->m_factoryIdentifier = m_processorFactory->getIdentifier();
    newNodeData->m_factoryVersion = m_processorFactory->getVersion();
    // TODO position

    projectGraphModel->scheduleCommand(std::make_unique<AddNodeCommand>("Add processor", std::move(newNodeData)));
}
