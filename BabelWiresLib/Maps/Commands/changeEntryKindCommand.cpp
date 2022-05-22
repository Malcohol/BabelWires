/**
 * The command which changes the kind of entry in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Maps/Commands/changeEntryKindCommand.hpp>

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProject.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <cassert>

babelwires::ChangeEntryKindCommand::ChangeEntryKindCommand(std::string commandName, MapEntryData::Kind kind,
                                                           unsigned int indexOfEntry)
    : SimpleCommand(commandName)
    , m_kind(kind)
    , m_indexOfEntry(indexOfEntry) {}

bool babelwires::ChangeEntryKindCommand::initialize(const MapProject& map) {
    const int numEntries = map.getNumMapEntries();
    if (m_indexOfEntry >= numEntries) {
        return false;
    }

    const bool isFallback =
        (m_kind == MapEntryData::Kind::Fallback_AllToOne) || (m_kind == MapEntryData::Kind::Fallback_AllToSame);
    const bool isLastEntry = (m_indexOfEntry == (numEntries - 1));
    if (isFallback != isLastEntry) {
        return false;
    }
    
    m_replacedEntry = map.getMapEntry(m_indexOfEntry).getData().clone();

    m_newEntry = MapEntryData::create(map.getProjectContext().m_typeSystem, map.getSourceTypeId(),
                                      map.getTargetTypeId(), m_kind);

    return true;
}

void babelwires::ChangeEntryKindCommand::execute(MapProject& map) const {
    map.replaceMapEntry(m_newEntry->clone(), m_indexOfEntry);
}

void babelwires::ChangeEntryKindCommand::undo(MapProject& map) const {
    map.replaceMapEntry(m_replacedEntry->clone(), m_indexOfEntry);
}
