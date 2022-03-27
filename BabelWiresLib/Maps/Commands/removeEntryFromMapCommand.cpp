/**
 * The command to remove entries from arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include "BabelWiresLib/Maps/Commands/removeEntryFromMapCommand.hpp"

#include "BabelWiresLib/Maps/mapProject.hpp"
#include "BabelWiresLib/Maps/mapProjectEntry.hpp"
#include "BabelWiresLib/Maps/MapEntries/mapEntryData.hpp"

#include <cassert>

babelwires::RemoveEntryFromMapCommand::RemoveEntryFromMapCommand(std::string commandName, unsigned int indexOfEntryToRemove)
    : SimpleCommand(commandName)
    , m_indexOfEntryToRemove(indexOfEntryToRemove) {}

bool babelwires::RemoveEntryFromMapCommand::initialize(const MapProject& map) {
    // You can't remove the last entry, since that should always be a fallback entry.
    if (m_indexOfEntryToRemove >= map.getNumMapEntries() - 1) {
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
