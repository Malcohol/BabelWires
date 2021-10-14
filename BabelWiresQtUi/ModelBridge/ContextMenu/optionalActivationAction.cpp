/**
 * QAction for activating or deactivating optional fields.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ContextMenu/optionalActivationAction.hpp"

#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"

#include "BabelWiresLib/Commands/activateOptionalCommand.hpp"
#include "BabelWiresLib/Commands/deactivateOptionalCommand.hpp"
#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"

babelwires::OptionalActivationAction::OptionalActivationAction(babelwires::FeaturePath pathToRecord,
                                                               Identifier optional, bool isActivated)
    : FeatureContextMenuAction(FieldNameRegistry::read()->getFieldName(optional).c_str())
    , m_pathToRecord(std::move(pathToRecord))
    , m_optional(optional)
    , m_isActivated(isActivated) {
        setCheckable(true);
        setChecked(isActivated);
    }

void babelwires::OptionalActivationAction::actionTriggered(babelwires::FeatureModel& model,
                                                           const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();
    std::unique_ptr<Command> command;
    std::string fieldName = FieldNameRegistry::read()->getFieldName(m_optional).c_str();
    if (!m_isActivated) {
        command = std::make_unique<ActivateOptionalCommand>("Activate optional field " + fieldName, elementId, m_pathToRecord, m_optional);
    } else {
        command = std::make_unique<DeactivateOptionalCommand>("Deactivate optional field " + fieldName, elementId, m_pathToRecord, m_optional);
    }
    projectBridge.scheduleCommand(std::move(command));
}
