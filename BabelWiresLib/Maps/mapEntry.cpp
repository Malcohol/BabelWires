/**
 * MapEntries define a relationship between values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapEntry.hpp>

#include <BabelWiresLib/Maps/mapEntryData.hpp>

babelwires::MapEntry::MapEntry(std::unique_ptr<MapEntryData> data)
    : m_data(std::move(data)) {}

babelwires::MapEntry::MapEntry(std::unique_ptr<MapEntryData> data, std::string reasonForFailure)
    : m_data(std::move(data))
    , m_reasonForFailure(std::move(reasonForFailure)) {}

babelwires::MapEntry::MapEntry(const MapEntry& other)
    : m_data(other.m_data->clone())
    , m_reasonForFailure(other.m_reasonForFailure) {}

babelwires::MapEntry::~MapEntry() = default;

const babelwires::MapEntryData& babelwires::MapEntry::getData() const {
    return *m_data;
}
