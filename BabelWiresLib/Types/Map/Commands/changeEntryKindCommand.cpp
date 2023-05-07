/**
 * The command which changes the kind of entry in a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresLib/Types/Map/Commands/changeEntryKindCommand.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
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

    const bool isFallback = MapEntryData::isFallback(m_kind);
    const bool isLastEntry = (m_indexOfEntry == (numEntries - 1));
    if (isFallback != isLastEntry) {
        return false;
    }

    m_replacedEntry = map.getMapEntry(m_indexOfEntry).getData().clone();

    m_newEntry = MapEntryData::create(map.getProjectContext().m_typeSystem, map.getSourceTypeRef(),
                                      map.getTargetTypeRef(), m_kind);

    return true;
}

void babelwires::ChangeEntryKindCommand::execute(MapProject& map) const {
    map.replaceMapEntry(m_newEntry->clone(), m_indexOfEntry);
}

void babelwires::ChangeEntryKindCommand::undo(MapProject& map) const {
    map.replaceMapEntry(m_replacedEntry->clone(), m_indexOfEntry);
}
