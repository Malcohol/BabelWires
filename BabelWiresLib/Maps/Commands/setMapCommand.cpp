/**
 * The command which sets a map to its default state
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Maps/Commands/setMapCommand.hpp>

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

babelwires::SetMapCommand::SetMapCommand(std::string commandName, std::unique_ptr<MapData> newData)
    : SimpleCommand(commandName), m_newContents(std::move(newData)) {}

bool babelwires::SetMapCommand::initialize(const MapProject& map) {
    m_oldContents = std::make_unique<MapData>(map.extractMapData());
    return true;
}

void babelwires::SetMapCommand::execute(MapProject& map) const {
    map.setMapData(*m_newContents);
}

void babelwires::SetMapCommand::undo(MapProject& map) const {
    map.setMapData(*m_oldContents);
}
