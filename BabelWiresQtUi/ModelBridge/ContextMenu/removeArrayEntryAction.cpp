/**
 * QAction for the remove entry from array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/removeArrayEntryAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/removeEntryFromArrayCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

babelwires::RemoveArrayEntryAction::RemoveArrayEntryAction(babelwires::Path pathToArray,
                                                           int indexOfEntryToRemove)
    : NodeContentsContextMenuActionBase(tr("Remove this array element"))
    , m_pathToArray(std::move(pathToArray))
    , m_indexOfEntryToRemove(indexOfEntryToRemove) {}

void babelwires::RemoveArrayEntryAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                         const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const NodeId elementId = model.getNodeId();
    projectBridge.scheduleCommand(std::make_unique<RemoveEntryFromArrayCommand>(
        "Remove element from array", elementId, m_pathToArray, m_indexOfEntryToRemove, 1));
}
