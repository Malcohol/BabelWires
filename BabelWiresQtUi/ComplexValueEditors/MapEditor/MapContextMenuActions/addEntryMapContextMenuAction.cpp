/**
 * This action adds an entry above the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/addEntryMapContextMenuAction.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>

#include <BabelWiresLib/Types/Map/Commands/addEntryToMapCommand.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

babelwires::AddEntryMapContextMenuAction::AddEntryMapContextMenuAction(const QString& text,
                                                                       unsigned int indexOfNewEntry)
    : ContextMenuAction(text), m_indexOfNewEntry(indexOfNewEntry) {}

void babelwires::AddEntryMapContextMenuAction::actionTriggered(QAbstractItemModel& model,
                                                               const QModelIndex& index) const {
    MapModel *const mapModel = qobject_cast<MapModel*>(&model);
    assert(mapModel && "Action used with wrong model");
    MapEditor& mapEditor = mapModel->getMapEditor();
    const MapProject& mapProject = mapEditor.getMapProject();

    auto newEntry = std::make_unique<OneToOneMapEntryData>(mapProject.getProjectContext().m_typeSystem, mapProject.getSourceTypeRef(), mapProject.getTargetTypeRef());
    mapEditor.executeCommand(std::make_unique<AddEntryToMapCommand>("Add entry above", std::move(newEntry), m_indexOfNewEntry));
}
