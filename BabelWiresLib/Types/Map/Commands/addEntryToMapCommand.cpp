/**
 * The command which adds entries to arrays.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/addEntryToMapCommand.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>

#include <cassert>

babelwires::AddEntryToMapCommand::AddEntryToMapCommand(std::string commandName, unsigned int indexOfNewEntry)
    : SimpleCommand(commandName)
    , m_indexOfNewEntry(indexOfNewEntry) {}

bool babelwires::AddEntryToMapCommand::initialize(const MapProject& map) {
    // You can't "add" to the last entry, since it should be always be a fallback entry.
    if (m_indexOfNewEntry >= map.getNumMapEntries()) {
        return false;
    }

    return true;
}

void babelwires::AddEntryToMapCommand::execute(MapProject& map) const {
    auto newEntry = std::make_unique<OneToOneMapEntryData>(
        map.getProjectContext().m_typeSystem, map.getCurrentSourceTypeRef(), map.getCurrentTargetTypeRef());
    map.addMapEntry(std::move(newEntry), m_indexOfNewEntry);
}

void babelwires::AddEntryToMapCommand::undo(MapProject& map) const {
    map.removeMapEntry(m_indexOfNewEntry);
}
