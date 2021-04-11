/**
 * QAction for the insert entry into array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresQtUi/ModelBridge/ContextMenu/insertArrayEntryAction.hpp"

#include "BabelWiresQtUi/ModelBridge/featureModel.hpp"
#include "BabelWiresQtUi/ModelBridge/projectBridge.hpp"

#include "BabelWires/Commands/addEntryToArrayCommand.hpp"

babelwires::InsertArrayEntryAction::InsertArrayEntryAction(babelwires::FeaturePath pathToArray, int indexOfNewEntry)
    : FeatureContextMenuAction((indexOfNewEntry == -1) ? tr("Add an element") : tr("Insert array element here"))
    , m_pathToArray(std::move(pathToArray))
    , m_indexOfNewEntry(indexOfNewEntry) {}

void babelwires::InsertArrayEntryAction::actionTriggered(babelwires::FeatureModel& model,
                                                         const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();
    projectBridge.scheduleCommand(
        std::make_unique<AddEntryToArrayCommand>("Add element to array", elementId, m_pathToArray, m_indexOfNewEntry));
}
