/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/targetFileNodeFactory.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp> 

babelwires::TargetFileNodeFactory::TargetFileNodeFactory(const UiProjectContext& uiProjectContext)
    : m_targetFileFormatRegistry(uiProjectContext.m_targetFileFormatReg) {}

QString babelwires::TargetFileNodeFactory::getCategoryName() const {
    return m_targetFileFormatRegistry.getRegistryName().c_str();
}

QList<QString> babelwires::TargetFileNodeFactory::getFactoryNames() const {
    QList<QString> factoryNames;
    auto identifierRegistry = IdentifierRegistry::read();
    for (const auto& entry : m_targetFileFormatRegistry) {
        factoryNames.append(identifierRegistry->getName(entry.getIdentifier()).c_str());
    }
    return factoryNames;
}

void babelwires::TargetFileNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName, QPointF scenePos) {
    const TargetFileFormat* targetFileFormat = m_targetFileFormatRegistry.getEntryByName(factoryName.toStdString());
    assert(targetFileFormat);

    auto newNodeData = std::make_unique<TargetFileNodeData>();
    newNodeData->m_factoryIdentifier = targetFileFormat->getIdentifier();
    newNodeData->m_factoryVersion = targetFileFormat->getVersion();
    newNodeData->m_uiData.m_uiPosition.m_x = scenePos.x();
    newNodeData->m_uiData.m_uiPosition.m_y = scenePos.y();

    projectGraphModel.scheduleCommand(std::make_unique<AddNodeCommand>("Add target file", std::move(newNodeData)));
}
