/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Maps/Commands/addEntryToMapCommand.hpp"

#include "BabelWiresLib/Maps/map.hpp"
#include "BabelWiresLib/Maps/mapEntry.hpp"

#include <cassert>

babelwires::AddEntryToMapCommand::AddEntryToMapCommand(std::string commandName, std::unique_ptr<MapEntry> newEntry, unsigned int indexOfNewEntry)
    : SimpleCommand(commandName)
    , m_newEntry(std::move(newEntry))
    , m_indexOfNewEntry(indexOfNewEntry) {}

bool babelwires::AddEntryToMapCommand::initialize(const Map& map) {
    if (m_indexOfNewEntry > map.getNumMapEntries()) {
        return false;
    }

    if (map.validateNewEntry(*m_newEntry)) {
        return false;
    }

    return true;
}

void babelwires::AddEntryToMapCommand::execute(Map& map) const {
    map.addMapEntry(m_newEntry->clone(), m_indexOfNewEntry);
}

void babelwires::AddEntryToMapCommand::undo(Map& map) const {
    map.removeMapEntry(m_indexOfNewEntry);
}
