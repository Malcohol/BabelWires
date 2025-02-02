/**
 * QAction for the remove failed modifiers action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/removeFailedModifiersAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/removeFailedModifiersCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Nodes/contentsCache.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <BabelWiresQtUi/ModelBridge/accessModelScope.hpp>

babelwires::RemoveFailedModifiersAction::RemoveFailedModifiersAction()
    : FeatureContextMenuAction(tr("Remove all failed modifiers from this feature")) {}

void babelwires::RemoveFailedModifiersAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                              const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const NodeId elementId = model.getNodeId();

    AccessModelScope scope(projectBridge);
    const Project& project = scope.getProject();
    const Node* const node = project.getNode(elementId);
    if (!node) {
        return;
    }

    const babelwires::ContentsCacheEntry* entry = model.getEntry(scope, index);
    if (!entry) {
        return;
    }
    projectBridge.scheduleCommand(
        std::make_unique<RemoveFailedModifiersCommand>("Remove failed modifiers", elementId, entry->getPath()));
}
