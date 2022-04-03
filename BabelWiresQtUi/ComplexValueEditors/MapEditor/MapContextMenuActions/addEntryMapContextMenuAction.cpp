/**
 * This action adds an entry above the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/addEntryMapContextMenuAction.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresLib/Maps/mapProject.hpp>

babelwires::AddEntryMapContextMenuAction::AddEntryMapContextMenuAction(const QString& text,
                                                                       unsigned int indexOfNewEntry)
    : ContextMenuAction("Add map above"), m_indexOfNewEntry(indexOfNewEntry) {}

void babelwires::AddEntryMapContextMenuAction::actionTriggered(QAbstractItemModel& model,
                                                               const QModelIndex& index) const {
    MapModel *const mapModel = qobject_cast<MapModel*>(&model);
    assert(mapModel && "Action used with wrong model");
    MapProject& mapProject = mapModel->getMapProject();
    
}
