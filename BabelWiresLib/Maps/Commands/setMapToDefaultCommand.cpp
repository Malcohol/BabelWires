/**
 * The command which sets a map to its default state
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Maps/Commands/setMapToDefaultCommand.hpp"

#include "BabelWiresLib/Maps/MapEntries/mapEntryData.hpp"
#include "BabelWiresLib/Maps/mapProject.hpp"
#include "BabelWiresLib/Maps/mapProjectEntry.hpp"

babelwires::SetMapToDefaultCommand::SetMapToDefaultCommand(std::string commandName)
    : SimpleCommand(commandName) {}

bool babelwires::SetMapToDefaultCommand::initialize(const MapProject& map) {
    m_oldContents = std::make_unique<MapData>(map.extractMapData());
    return true;
}

void babelwires::SetMapToDefaultCommand::execute(MapProject& map) const {
    MapData defaultMap;
    defaultMap.setSourceTypeId(map.getAllowedSourceTypeId());
    defaultMap.setTargetTypeId(map.getAllowedTargetTypeId());
    defaultMap.setEntriesToDefault(map.getProjectContext());
    map.setMapData(defaultMap);
}

void babelwires::SetMapToDefaultCommand::undo(MapProject& map) const {
    map.setMapData(*m_oldContents);
}
