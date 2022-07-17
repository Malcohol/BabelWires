/**
 * QAction for the insert entry into array action in the context menu.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ModelBridge/ContextMenu/insertArrayEntryAction.hpp>

#include <BabelWiresQtUi/ModelBridge/featureModel.hpp>
#include <BabelWiresQtUi/ModelBridge/projectBridge.hpp>

#include <BabelWiresLib/Project/Commands/addEntryToArrayCommand.hpp>

babelwires::InsertArrayEntryAction::InsertArrayEntryAction(const QString& text, babelwires::FeaturePath pathToArray, int indexOfNewEntry)
    : FeatureContextMenuAction(text)
    , m_pathToArray(std::move(pathToArray))
    , m_indexOfNewEntry(indexOfNewEntry) {}

void babelwires::InsertArrayEntryAction::actionTriggered(babelwires::FeatureModel& model,
                                                         const QModelIndex& index) const {
    ProjectBridge& projectBridge = model.getProjectBridge();
    const ElementId elementId = model.getElementId();
    projectBridge.scheduleCommand(
        std::make_unique<AddEntryToArrayCommand>("Add element to array", elementId, m_pathToArray, m_indexOfNewEntry));
}
