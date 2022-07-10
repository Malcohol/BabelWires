/**
 * A MapProjectEntry wraps MapEntryData and augments it with features relevant for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>

babelwires::MapProjectEntry::MapProjectEntry(std::unique_ptr<MapEntryData> data)
    : m_data(std::move(data)), m_validityOfEntry(Result::success) {}

babelwires::MapProjectEntry::MapProjectEntry(const MapProjectEntry& other)
    : m_data(other.m_data->clone())
    , m_validityOfEntry(other.m_validityOfEntry) {}

babelwires::MapProjectEntry::~MapProjectEntry() = default;

const babelwires::MapEntryData& babelwires::MapProjectEntry::getData() const {
    return *m_data;
}

babelwires::Result babelwires::MapProjectEntry::getValidity() const {
    return m_validityOfEntry;
}

void babelwires::MapProjectEntry::validate(const TypeSystem& typeSystem, LongIdentifier sourceTypeId,
                                           LongIdentifier targetTypeId, bool isLastEntry) {
    m_validityOfEntry = getData().validate(typeSystem, sourceTypeId, targetTypeId, isLastEntry);
}
