/**
 * QAction for activating or deactivating optional fields.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/optionalActivationAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/activateOptionalCommand.hpp>
#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

babelwires::OptionalActivationAction::OptionalActivationAction(babelwires::Path pathToRecord,
                                                               ShortId optional, bool isActivated)
    : NodeContentsContextMenuActionBase(IdentifierRegistry::read()->getName(optional).c_str())
    , m_pathToRecord(std::move(pathToRecord))
    , m_optional(optional)
    , m_isActivated(isActivated) {
        setCheckable(true);
        setChecked(isActivated);
    }

void babelwires::OptionalActivationAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                           const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const NodeId elementId = model.getNodeId();
    std::unique_ptr<Command<Project>> command;
    std::string fieldName = IdentifierRegistry::read()->getName(m_optional).c_str();
    if (!m_isActivated) {
        command = std::make_unique<ActivateOptionalCommand>("Activate optional field " + fieldName, elementId, m_pathToRecord, m_optional);
    } else {
        command = std::make_unique<DeactivateOptionalCommand>("Deactivate optional field " + fieldName, elementId, m_pathToRecord, m_optional);
    }
    projectBridge.scheduleCommand(std::move(command));
}
