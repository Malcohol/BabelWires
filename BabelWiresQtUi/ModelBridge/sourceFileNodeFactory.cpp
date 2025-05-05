/**
 * The factory which creates nodes for source files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/sourceFileNodeFactory.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeNodeModel.hpp>
#include <BabelWiresQtUi/NodeEditorBridge/projectGraphModel.hpp>
#include <BabelWiresQtUi/Utilities/fileDialogs.hpp>

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>
#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>

#include <nodes/FlowScene>

babelwires::SourceFileNodeFactory::SourceFileNodeFactory(ProjectGraphModel* projectGraphModel, const SourceFileFormat* sourceFileFormat)
    : NodeFactoryBase(projectGraphModel)
    , m_sourceFileFormat(sourceFileFormat) {}

QString babelwires::SourceFileNodeFactory::name() const {
    return m_sourceFileFormat->getName().c_str();
}

std::unique_ptr<QtNodes::NodeDataModel> babelwires::SourceFileNodeFactory::createNode() const {
    QString filePath = showOpenFileDialog(m_projectGraphModel->getFlowGraphWidget(), *m_sourceFileFormat);

    if (!filePath.isNull()) {
        auto newDataPtr = std::make_unique<SourceFileNodeData>();
        newDataPtr->m_factoryIdentifier = m_sourceFileFormat->getIdentifier();
        newDataPtr->m_filePath = filePath.toStdString();
        newDataPtr->m_factoryVersion = m_sourceFileFormat->getVersion();

        auto commandPtr = std::make_unique<AddNodeCommand>("Add source file", std::move(newDataPtr));
        AddNodeCommand& addNodeCommand = *commandPtr;
        if (m_projectGraphModel->executeAddNodeCommand(std::move(commandPtr))) {
            return std::make_unique<NodeNodeModel>(*m_projectGraphModel, addNodeCommand.getNodeId());
        }
    }
    return nullptr;
}
