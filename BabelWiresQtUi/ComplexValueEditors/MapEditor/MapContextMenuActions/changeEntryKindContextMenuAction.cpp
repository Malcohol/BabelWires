/**
 * This action changes the kind of entry at the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/changeEntryKindContextMenuAction.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>

#include <BabelWiresLib/Types/Map/Commands/changeEntryKindCommand.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

babelwires::ChangeEntryKindContextMenuAction::ChangeEntryKindContextMenuAction(const QString& text, MapEntryData::Kind kind,
                                                                       unsigned int indexOfNewEntry, bool isCheckable)
    : ContextMenuAction(text), m_kind(kind), m_indexOfNewEntry(indexOfNewEntry) {
        if (isCheckable) {
            setCheckable(true);
            setChecked(false);
        }
    }

void babelwires::ChangeEntryKindContextMenuAction::actionTriggered(QAbstractItemModel& model,
                                                               const QModelIndex& index) const {
    MapModel *const mapModel = qobject_cast<MapModel*>(&model);
    assert(mapModel && "Action used with wrong model");
    MapEditor& mapEditor = mapModel->getMapEditor();

    mapEditor.executeCommand(std::make_unique<ChangeEntryKindCommand>("Add entry above", m_kind, m_indexOfNewEntry));
}
