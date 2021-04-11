/**
 * QAction for the remove entry from array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ContextMenu/removeArrayEntryAction.hpp"

#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"

#include "BabelWires/Commands/removeEntryFromArrayCommand.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"

babelwires::RemoveArrayEntryAction::RemoveArrayEntryAction(babelwires::FeaturePath pathToArray,
                                                           int indexOfEntryToRemove)
    : FeatureContextMenuAction(tr("Remove this array element"))
    , m_pathToArray(std::move(pathToArray))
    , m_indexOfEntryToRemove(indexOfEntryToRemove) {}

void babelwires::RemoveArrayEntryAction::actionTriggered(babelwires::FeatureModel& model,
                                                         const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();
    projectBridge.scheduleCommand(std::make_unique<RemoveEntryFromArrayCommand>(
        "Remove element from array", elementId, m_pathToArray, m_indexOfEntryToRemove, 1));
}
