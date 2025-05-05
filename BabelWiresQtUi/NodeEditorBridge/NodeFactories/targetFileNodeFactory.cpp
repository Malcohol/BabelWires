/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/targetFileNodeFactory.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeNodeModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>

babelwires::TargetFileNodeFactory::TargetFileNodeFactory(const TargetFileFormatRegistry* targetFileFormat)
    : m_targetFileFormatRegistry(targetFileFormatRegistry) {}

QString babelwires::TargetFileNodeFactory::name() const {
    return m_targetFileFormatRegistry.getRegistryName().c_str();
}

QList<QString> babelwires::TargetFileNodeFactory::getFactoryNames() const {
    // TODO
}

void babelwires::TargetFileNodeFactory::createNode(ProjectGraphModel& projectGraphModel, QString factoryName,
    const TargetFileFormatFactory* targetFileFormatFactory = m_targetFileFormatRegistry.getEntryByName(factoryName);
    assert(targetFileFormatFactory);

    QPointF const scenePos, QWidget* parentForDialogs) {
    auto newDataPtr = std::make_unique<TargetFileNodeData>();
    newDataPtr->m_factoryIdentifier = m_targetFileFormat->getIdentifier();
    newDataPtr->m_factoryVersion = m_targetFileFormat->getVersion();
    // TODO position

    projectGraphModel->executeAddNodeCommand(std::make_unique<AddNodeCommand>("Add target file", std::move(newDataPtr)));
}
