/**
 * QAction for the insert entry into array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/insertArrayEntryAction.hpp>

#include <BabelWiresQtUi/ModelBridge/nodeContentsModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>

babelwires::InsertArrayEntryAction::InsertArrayEntryAction(const QString& text, babelwires::Path pathToArray, int indexOfNewEntry)
    : NodeContentsContextMenuActionBase(text)
    , m_pathToArray(std::move(pathToArray))
    , m_indexOfNewEntry(indexOfNewEntry) {}

void babelwires::InsertArrayEntryAction::actionTriggered(babelwires::NodeContentsModel& model,
                                                         const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const NodeId elementId = model.getNodeId();
    projectBridge.scheduleCommand(
        std::make_unique<AddEntriesToArrayCommand>("Add element to array", elementId, m_pathToArray, m_indexOfNewEntry));
}
