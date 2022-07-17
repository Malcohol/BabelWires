/**
 * QAction for the remove failed modifiers action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/removeFailedModifiersAction.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/removeFailedModifiersCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>

babelwires::RemoveFailedModifiersAction::RemoveFailedModifiersAction()
    : FeatureContextMenuAction(tr("Remove all failed modifiers from this feature")) {}

void babelwires::RemoveFailedModifiersAction::actionTriggered(babelwires::FeatureModel& model,
                                                              const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();

    AccessModelScope scope(projectBridge);
    const Project& project = scope.getProject();
    const FeatureElement* const element = project.getFeatureElement(elementId);
    if (!element) {
        return;
    }

    const babelwires::ContentsCacheEntry* entry = model.getEntry(scope, index);
    if (!entry) {
        return;
    }
    projectBridge.scheduleCommand(
        std::make_unique<RemoveFailedModifiersCommand>("Remove failed modifiers", elementId, entry->getPath()));
}
