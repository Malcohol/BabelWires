/**
 * The factory which creates nodes for target files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/targetFileNodeFactory.hpp"
#include "BabelWiresLib/Commands/addElementCommand.hpp"
#include "BabelWiresLib/FileFormat/targetFileFormat.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElementData.hpp"
#include "BabelWiresQtUi/ModelBridge/elementNodeModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"

#include <QtWidgets/QFileDialog>
#include <nodes/FlowScene>

babelwires::TargetFileNodeFactory::TargetFileNodeFactory(ProjectBridge* projectBridge,
                                                 const TargetFileFormat* targetFileFormat)
    : m_projectBridge(projectBridge)
    , m_targetFileFormat(targetFileFormat) {}

QString babelwires::TargetFileNodeFactory::name() const {
    return m_targetFileFormat->getName().c_str();
}

std::unique_ptr<QtNodes::NodeDataModel> babelwires::TargetFileNodeFactory::operator()() const {
    if (!m_queryHack) {
        m_queryHack = true;
        return std::make_unique<FactoryNameQuery>(*m_projectBridge, m_targetFileFormat->getName().c_str());
    }

    auto newDataPtr = std::make_unique<TargetFileElementData>();
    newDataPtr->m_factoryIdentifier = m_targetFileFormat->getIdentifier();
    newDataPtr->m_factoryVersion = m_targetFileFormat->getVersion();

    auto commandPtr = std::make_unique<AddElementCommand>("Add target file", std::move(newDataPtr));
    AddElementCommand& addElementCommand = *commandPtr;
    if (m_projectBridge->executeAddElementCommand(std::move(commandPtr))) {
        return std::make_unique<ElementNodeModel>(*m_projectBridge, addElementCommand.getElementId());
    }
    return nullptr;
}
