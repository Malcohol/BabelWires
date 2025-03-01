/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/replaceMapEntryCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>

#include <cassert>

babelwires::ReplaceMapEntryCommand::ReplaceMapEntryCommand(std::string commandName,
                                                           std::unique_ptr<MapEntryData> newEntry,
                                                           unsigned int indexOfReplacement)
    : SimpleCommand(commandName)
    , m_newEntry(std::move(newEntry))
    , m_indexOfReplacement(indexOfReplacement) {
    assert(m_newEntry);
}

babelwires::ReplaceMapEntryCommand::ReplaceMapEntryCommand(const ReplaceMapEntryCommand& other)
    : SimpleCommand(other)
    , m_newEntry(other.m_newEntry->clone())
    , m_replacedEntry(other.m_replacedEntry ? other.m_replacedEntry->clone() : nullptr)
    , m_indexOfReplacement(other.m_indexOfReplacement) {}

bool babelwires::ReplaceMapEntryCommand::initialize(const MapProject& map) {
    const int numEntries = map.getNumMapEntries();
    if (m_indexOfReplacement >= numEntries) {
        return false;
    }

    if (!map.validateNewEntry(*m_newEntry, (m_indexOfReplacement == (numEntries - 1)))) {
        return false;
    }

    m_replacedEntry = map.getMapEntry(m_indexOfReplacement).getData().clone();

    return true;
}

void babelwires::ReplaceMapEntryCommand::execute(MapProject& map) const {
    map.replaceMapEntry(m_newEntry->clone(), m_indexOfReplacement);
}

void babelwires::ReplaceMapEntryCommand::undo(MapProject& map) const {
    map.replaceMapEntry(m_replacedEntry->clone(), m_indexOfReplacement);
}
