/**
 * The command to remove entries from arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Maps/Commands/removeEntryFromMapCommand.hpp"

#include "BabelWiresLib/Maps/map.hpp"
#include "BabelWiresLib/Maps/mapEntry.hpp"
#include "BabelWiresLib/Maps/mapEntryData.hpp"

#include <cassert>

babelwires::RemoveEntryFromMapCommand::RemoveEntryFromMapCommand(std::string commandName, unsigned int indexOfEntryToRemove)
    : SimpleCommand(commandName)
    , m_indexOfEntryToRemove(indexOfEntryToRemove) {}

bool babelwires::RemoveEntryFromMapCommand::initialize(const MapProject& map) {
    if (m_indexOfEntryToRemove >= map.getNumMapEntries()) {
        return false;
    }

    m_removedEntry = map.getMapEntry(m_indexOfEntryToRemove).getData().clone();

    return true;
}

void babelwires::RemoveEntryFromMapCommand::execute(MapProject& map) const {
    map.removeMapEntry(m_indexOfEntryToRemove);
}

void babelwires::RemoveEntryFromMapCommand::undo(MapProject& map) const {
    map.addMapEntry(m_removedEntry->clone(), m_indexOfEntryToRemove);
}
