/**
 * QAction for activating or deactivating optional fields.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/selectVariantAction.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/selectUnionBranchCommand.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

babelwires::SelectVariantAction::SelectVariantAction(babelwires::FeaturePath pathToRecord, ShortId optional)
    : FeatureContextMenuAction(IdentifierRegistry::read()->getName(optional).c_str())
    , m_pathToRecord(std::move(pathToRecord))
    , m_tag(optional) {
    setCheckable(true);
}

void babelwires::SelectVariantAction::actionTriggered(babelwires::FeatureModel& model, const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();
    std::unique_ptr<Command<Project>> command;
    std::string fieldName = IdentifierRegistry::read()->getName(m_tag).c_str();
    command =
        std::make_unique<SelectUnionBranchCommand>("Select variant " + fieldName, elementId, m_pathToRecord, m_tag);
    projectBridge.scheduleCommand(std::move(command));
}
