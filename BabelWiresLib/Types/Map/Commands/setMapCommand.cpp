/**
 * The command which sets a map to its default state
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/setMapCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

babelwires::SetMapCommand::SetMapCommand(std::string commandName, ValueHolder newData)
    : SimpleCommand(commandName)
    , m_newContents(std::move(newData)) {
    assert(m_newContents->as<MapValue>() && "SetMapCommand given non-map value");
}

bool babelwires::SetMapCommand::initialize(const MapProject& map) {
    m_oldContents = map.extractMapValue();
    return true;
}

void babelwires::SetMapCommand::execute(MapProject& map) const {
    map.setMapValue(m_newContents->is<MapValue>());
}

void babelwires::SetMapCommand::undo(MapProject& map) const {
    map.setMapValue(m_oldContents->is<MapValue>());
}
