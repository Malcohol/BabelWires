/**
 * This action removes the entry at the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/removeEntryMapContextMenuAction.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>

#include <BabelWiresLib/Types/Map/Commands/removeEntryFromMapCommand.hpp>

babelwires::RemoveEntryMapContextMenuAction::RemoveEntryMapContextMenuAction(const QString& text,
                                                                       unsigned int indexOfEntry)
    : ContextMenuAction(text), m_indexOfEntry(indexOfEntry) {}

void babelwires::RemoveEntryMapContextMenuAction::actionTriggered(QAbstractItemModel& model,
                                                               const QModelIndex& index) const {
    MapModel *const mapModel = qobject_cast<MapModel*>(&model);
    assert(mapModel && "Action used with wrong model");
    MapEditor& mapEditor = mapModel->getMapEditor();
    const MapProject& mapProject = mapEditor.getMapProject();

    mapEditor.executeCommand(std::make_unique<RemoveEntryFromMapCommand>("Remove map entry", m_indexOfEntry));
}
