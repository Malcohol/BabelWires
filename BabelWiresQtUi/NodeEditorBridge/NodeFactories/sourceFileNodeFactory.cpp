/**
 * The factory which creates nodes for source files.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/NodeEditorBridge/NodeFactories/sourceFileNodeFactory.hpp>

#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>
#include <BabelWiresQtUi/uiProjectContext.hpp>

#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp> 

babelwires::SourceFileNodeFactory::SourceFileNodeFactory(const UiProjectContext& projectContext)
    : m_sourceFileFormatRegistry(projectContext.m_sourceFileFormatReg) {}

QString babelwires::SourceFileNodeFactory::getCategoryName() const {
    return m_sourceFileFormatRegistry.getRegistryName().c_str();
}

QList<QString> babelwires::SourceFileNodeFactory::getFactoryNames() const {
    QList<QString> factoryNames;
    auto identifierRegistry = IdentifierRegistry::read();
    for (const auto& entry : m_sourceFileFormatRegistry) {
        factoryNames.append(identifierRegistry->getName(entry.getIdentifier()).c_str());
    }
    return factoryNames;
}

void babelwires::SourceFileNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName,
                                                   QPointF scenePos) {
    const SourceFileFormat* sourceFileFormat = m_sourceFileFormatRegistry.getEntryByName(factoryName.toStdString());
    assert(sourceFileFormat);

    QString filePath = showOpenFileDialog(projectGraphModel.getFlowGraphWidget(), *sourceFileFormat);

    if (!filePath.isNull()) {
        auto newNodeData = std::make_unique<SourceFileNodeData>();
        newNodeData->m_factoryIdentifier = sourceFileFormat->getIdentifier();
        newNodeData->m_filePath = filePath.toStdString();
        newNodeData->m_factoryVersion = sourceFileFormat->getVersion();
        newNodeData->m_uiData.m_uiPosition.m_x = scenePos.x();
        newNodeData->m_uiData.m_uiPosition.m_y = scenePos.y();

        projectGraphModel.scheduleCommand(std::make_unique<AddNodeCommand>("Add source file", std::move(newNodeData)));
    }
}
