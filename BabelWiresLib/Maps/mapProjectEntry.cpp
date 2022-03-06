/**
 * MapEntries define a relationship between values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>

#include <BabelWiresLib/Maps/mapEntryData.hpp>

babelwires::MapProjectEntry::MapProjectEntry(std::unique_ptr<MapEntryData> data)
    : m_data(std::move(data)) {}

babelwires::MapProjectEntry::MapProjectEntry(std::unique_ptr<MapEntryData> data, std::string reasonForFailure)
    : m_data(std::move(data))
    , m_reasonForFailure(std::move(reasonForFailure)) {}

babelwires::MapProjectEntry::MapProjectEntry(const MapProjectEntry& other)
    : m_data(other.m_data->clone())
    , m_reasonForFailure(other.m_reasonForFailure) {}

babelwires::MapProjectEntry::~MapProjectEntry() = default;

const babelwires::MapEntryData& babelwires::MapProjectEntry::getData() const {
    return *m_data;
}
