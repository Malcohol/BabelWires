/**
 * The command which sets a map to its default state
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/setMapCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapProject.hpp>
#include <BabelWiresLib/Types/Map/mapProjectEntry.hpp>

babelwires::SetMapCommand::SetMapCommand(std::string commandName, std::unique_ptr<MapValue> newData)
    : SimpleCommand(commandName), m_newContents(std::move(newData)) {}

bool babelwires::SetMapCommand::initialize(const MapProject& map) {
    m_oldContents = std::make_unique<MapValue>(map.extractMapValue());
    return true;
}

void babelwires::SetMapCommand::execute(MapProject& map) const {
    map.setMapValue(*m_newContents);
}

void babelwires::SetMapCommand::undo(MapProject& map) const {
    map.setMapValue(*m_oldContents);
}
