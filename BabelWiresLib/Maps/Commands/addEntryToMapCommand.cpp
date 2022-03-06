/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Maps/Commands/addEntryToMapCommand.hpp"

#include "BabelWiresLib/Maps/mapProject.hpp"
#include "BabelWiresLib/Maps/MapEntries/mapEntryData.hpp"

#include <cassert>

babelwires::AddEntryToMapCommand::AddEntryToMapCommand(std::string commandName, std::unique_ptr<MapEntryData> newEntry, unsigned int indexOfNewEntry)
    : SimpleCommand(commandName)
    , m_newEntry(std::move(newEntry))
    , m_indexOfNewEntry(indexOfNewEntry) {}

bool babelwires::AddEntryToMapCommand::initialize(const MapProject& map) {
    if (m_indexOfNewEntry > map.getNumMapEntries()) {
        return false;
    }

    if (!map.validateNewEntry(*m_newEntry)) {
        return false;
    }

    return true;
}

void babelwires::AddEntryToMapCommand::execute(MapProject& map) const {
    map.addMapEntry(m_newEntry->clone(), m_indexOfNewEntry);
}

void babelwires::AddEntryToMapCommand::undo(MapProject& map) const {
    map.removeMapEntry(m_indexOfNewEntry);
}
