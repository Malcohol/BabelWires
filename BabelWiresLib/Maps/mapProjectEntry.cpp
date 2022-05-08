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
    : m_data(std::move(data)) {}

babelwires::MapProjectEntry::MapProjectEntry(std::unique_ptr<MapEntryData> data, Result validity)
    : m_data(std::move(data))
    , m_validityOfEntry(std::move(validity)) {}

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

void babelwires::MapProjectEntry::validate(const Type& sourceType, const Type& targetType) {
    m_validityOfEntry = getData().validate(sourceType, targetType);
}
