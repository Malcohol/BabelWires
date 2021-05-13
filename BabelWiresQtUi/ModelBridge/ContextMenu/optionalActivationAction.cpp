/**
 * QAction for the insert entry into array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ContextMenu/optionalActivationAction.hpp"

#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"

#include "BabelWires/Commands/activateOptionalsCommand.hpp"
#include "BabelWires/Features/Path/fieldNameRegistry.hpp"

babelwires::OptionalActivationAction::OptionalActivationAction(babelwires::FeaturePath pathToRecord,
                                                               FieldIdentifier optional, bool isActivated)
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
    if (!m_isActivated) {
        // TODO Add name.
        command = std::make_unique<ActivateOptionalsCommand>("Activate optional", elementId, m_pathToRecord, m_optional);
    } else {
        // TODO
    }
    projectBridge.scheduleCommand(std::move(command));
}
