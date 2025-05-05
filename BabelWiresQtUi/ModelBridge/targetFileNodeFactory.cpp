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

#include <QtWidgets/QFileDialog>
#include <nodes/FlowScene>

babelwires::TargetFileNodeFactory::TargetFileNodeFactory(ProjectGraphModel* projectGraphModel,
                                                 const TargetFileFormat* targetFileFormat)
    : NodeFactoryBase(projectGraphModel)
    , m_targetFileFormat(targetFileFormat) {}

QString babelwires::TargetFileNodeFactory::name() const {
    return m_targetFileFormat->getName().c_str();
}

std::unique_ptr<QtNodes::NodeDataModel> babelwires::TargetFileNodeFactory::createNode() const {
    auto newDataPtr = std::make_unique<TargetFileNodeData>();
    newDataPtr->m_factoryIdentifier = m_targetFileFormat->getIdentifier();
    newDataPtr->m_factoryVersion = m_targetFileFormat->getVersion();

    auto commandPtr = std::make_unique<AddNodeCommand>("Add target file", std::move(newDataPtr));
    AddNodeCommand& addNodeCommand = *commandPtr;
    if (m_projectGraphModel->executeAddNodeCommand(std::move(commandPtr))) {
        return std::make_unique<NodeNodeModel>(*m_projectGraphModel, addNodeCommand.getNodeId());
    }
    return nullptr;
}
