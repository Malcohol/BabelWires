/**
 * The factory which creates nodes for source files.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/sourceFileNodeFactory.hpp"

#include "BabelWiresQtUi/ModelBridge/elementNodeModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"
#include "BabelWiresQtUi/Utilities/fileDialogs.hpp"

#include "BabelWiresLib/Project/Commands/addElementCommand.hpp"
#include "BabelWiresLib/FileFormat/sourceFileFormat.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElementData.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"

#include <nodes/FlowScene>

babelwires::SourceFileNodeFactory::SourceFileNodeFactory(ProjectBridge* projectBridge, const SourceFileFormat* sourceFileFormat)
    : m_projectBridge(projectBridge)
    , m_sourceFileFormat(sourceFileFormat) {}

QString babelwires::SourceFileNodeFactory::name() const {
    return m_sourceFileFormat->getName().c_str();
}

std::unique_ptr<QtNodes::NodeDataModel> babelwires::SourceFileNodeFactory::operator()() const {
    if (!m_queryHack) {
        m_queryHack = true;
        return std::make_unique<FactoryNameQuery>(*m_projectBridge, m_sourceFileFormat->getName().c_str());
    }

    QString filePath = showOpenFileDialog(m_projectBridge->getFlowGraphWidget(), *m_sourceFileFormat);

    if (!filePath.isNull()) {
        auto newDataPtr = std::make_unique<SourceFileElementData>();
        newDataPtr->m_factoryIdentifier = m_sourceFileFormat->getIdentifier();
        newDataPtr->m_filePath = filePath.toStdString();
        newDataPtr->m_factoryVersion = m_sourceFileFormat->getVersion();

        auto commandPtr = std::make_unique<AddElementCommand>("Add source file", std::move(newDataPtr));
        AddElementCommand& addElementCommand = *commandPtr;
        if (m_projectBridge->executeAddElementCommand(std::move(commandPtr))) {
            return std::make_unique<ElementNodeModel>(*m_projectBridge, addElementCommand.getElementId());
        }
    }
    return nullptr;
}
