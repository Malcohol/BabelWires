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

#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>

babelwires::SourceFileNodeFactory::SourceFileNodeFactory(const SourceFileFormatRegistry* sourceFileFormat)
    : m_sourceFileFormatRegistry(sourceFileFormatRegistry) {}

QString babelwires::SourceFileNodeFactory::name() const {
    return m_sourceFileFormatRegistry.getRegistryName().c_str();
}

QList<QString> babelwires::SourceFileNodeFactory::getFactoryNames() const {
    // TODO
}

void babelwires::SourceFileNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName,
                                                   QPointF const scenePos, QWidget* parentForDialogs) {
    const SourceFileFormatFactory* sourceFileFormatFactory = m_sourceFileFormatRegistry.getEntryByName(factoryName);
    assert(sourceFileFormatFactory);

    QString filePath = showOpenFileDialog(m_projectGraphModel->getFlowGraphWidget(), *m_sourceFileFormat);

    if (!filePath.isNull()) {
        auto newDataPtr = std::make_unique<SourceFileNodeData>();
        newDataPtr->m_factoryIdentifier = sourceFileFormatFactory->getIdentifier();
        newDataPtr->m_filePath = filePath.toStdString();
        newDataPtr->m_factoryVersion = sourceFileFormatFactory->getVersion();
        // TODO position

        projectGraphModel->scheduleCommand(std::make_unique<AddNodeCommand>("Add source file", std::move(newDataPtr)));
    }
}
