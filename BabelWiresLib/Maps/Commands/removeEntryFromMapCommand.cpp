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

#include <cassert>

babelwires::RemoveEntryFromMapCommand::RemoveEntryFromMapCommand(std::string commandName, unsigned int indexOfEntryToRemove)
    : SimpleCommand(commandName)
    , m_indexOfEntryToRemove(indexOfEntryToRemove) {}

bool babelwires::RemoveEntryFromMapCommand::initialize(const Map& map) {
    if (m_indexOfEntryToRemove >= map.getNumMapEntries()) {
        return false;
    }

    m_removedEntry = map.getMapEntry(m_indexOfEntryToRemove).clone();

    return true;
}

void babelwires::RemoveEntryFromMapCommand::execute(Map& map) const {
    map.removeMapEntry(m_indexOfEntryToRemove);
}

void babelwires::RemoveEntryFromMapCommand::undo(Map& map) const {
    map.addMapEntry(m_removedEntry->clone(), m_indexOfEntryToRemove);
}
