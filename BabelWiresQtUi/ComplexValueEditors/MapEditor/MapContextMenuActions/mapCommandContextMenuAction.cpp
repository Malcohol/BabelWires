/**
 * This action changes the kind of entry at the current item.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/MapContextMenuActions/mapCommandContextMenuAction.hpp>

#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapEditor.hpp>
#include <BabelWiresQtUi/ComplexValueEditors/MapEditor/mapModel.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Map/Commands/changeEntryKindCommand.hpp>

babelwires::MapCommandContextMenuAction::MapCommandContextMenuAction(std::unique_ptr<Command<MapProject>> command)
    : ContextMenuAction(command->getName().c_str())
    , m_command(std::move(command)) {
    assert(m_command && "Command cannot be null");
}

void babelwires::MapCommandContextMenuAction::actionTriggered(QAbstractItemModel& model,
                                                              const QModelIndex& index) const {
    assert(m_command && "Command was null when MapCommandContextMenuAction triggered");
    MapModel* const mapModel = qobject_cast<MapModel*>(&model);
    assert(mapModel && "Action used with wrong model");
    MapEditor& mapEditor = mapModel->getMapEditor();
    mapEditor.executeCommand(m_command->clone());
}
