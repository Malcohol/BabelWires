/**
 * The factory which creates nodes for processors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/processorNodeFactory.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp> 

babelwires::ProcessorNodeFactory::ProcessorNodeFactory(const UiProjectContext& projectContext)
    : m_processorFactoryRegistry(projectContext.m_processorReg) {}

QString babelwires::ProcessorNodeFactory::getCategoryName() const {
    return m_processorFactoryRegistry.getRegistryName().c_str();
}

QList<QString> babelwires::ProcessorNodeFactory::getFactoryNames() const {
    QList<QString> factoryNames;
    auto identifierRegistry = IdentifierRegistry::read();
    for (const auto& entry : m_processorFactoryRegistry) {
        factoryNames.append(identifierRegistry->getName(entry.getIdentifier()).c_str());
    }
    return factoryNames;
}

void babelwires::ProcessorNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName,
                                                  QPointF scenePos) {
    const ProcessorFactory* processorFactory = m_processorFactoryRegistry.getEntryByName(factoryName.toStdString());
    assert(processorFactory);

    auto newNodeData = std::make_unique<ProcessorNodeData>();
    newNodeData->m_factoryIdentifier = processorFactory->getIdentifier();
    newNodeData->m_factoryVersion = processorFactory->getVersion();
    newNodeData->m_uiData.m_uiPosition.m_x = scenePos.x();
    newNodeData->m_uiData.m_uiPosition.m_y = scenePos.y();

    projectGraphModel.scheduleCommand(std::make_unique<AddNodeCommand>("Add processor", std::move(newNodeData)));
}
