/**
 * A Map defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/map.hpp>

#include <BabelWiresLib/Maps/mapEntryData.hpp>
#include <BabelWiresLib/Maps/mapEntry.hpp>
#include <BabelWiresLib/Maps/typeSystem.hpp>

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

babelwires::Map::Map()
    : m_sourceId(getBuiltInTypeId(KnownType::Int))
    , m_targetId(getBuiltInTypeId(KnownType::Int)) {}

babelwires::Map::Map(const Map& other)
    : m_sourceId(other.m_sourceId)
    , m_targetId(other.m_targetId) {
    for (const auto& e : other.m_mapEntries) {
        m_mapEntries.emplace_back(e->clone());
    }
}

babelwires::Map::Map(Map&& other)
    : m_sourceId(other.m_sourceId)
    , m_targetId(other.m_targetId)
    , m_mapEntries(std::move(other.m_mapEntries)) {}

babelwires::Map& babelwires::Map::operator=(const Map& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    for (const auto& e : other.m_mapEntries) {
        m_mapEntries.emplace_back(e->clone());
    }
    return *this;
}

babelwires::Map& babelwires::Map::operator=(Map&& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    m_mapEntries = std::move(other.m_mapEntries);
    return *this;
}

babelwires::Map::~Map() = default;

babelwires::LongIdentifier babelwires::Map::getSourceId() const {
    return m_sourceId;
}

babelwires::LongIdentifier babelwires::Map::getTargetId() const {
    return m_targetId;
}

void babelwires::Map::setSourceId(LongIdentifier sourceId) {
    m_sourceId = sourceId;
}

void babelwires::Map::setTargetId(LongIdentifier targetId) {
    m_targetId = targetId;
}

bool babelwires::Map::operator==(const Map& other) const {
    if ((m_sourceId != other.m_sourceId) || (m_targetId != other.m_targetId)) {
        return false;
    }
    return m_mapEntries == other.m_mapEntries;
}

bool babelwires::Map::operator!=(const Map& other) const {
    if ((m_sourceId != other.m_sourceId) || (m_targetId != other.m_targetId)) {
        return true;
    }
    return m_mapEntries != other.m_mapEntries;
}

unsigned int babelwires::Map::getNumMapEntries() const {
    return m_mapEntries.size();
}

const babelwires::MapEntry& babelwires::Map::getMapEntry(unsigned int index) const {
    return *m_mapEntries[index];
}

bool babelwires::Map::validateNewEntry(const MapEntryData& newEntry) const {
    return MapData::validateEntryData(m_sourceId, m_targetId, newEntry).empty();
}

void babelwires::Map::addMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index) {
    assert(validateNewEntry(*newEntry) && "The new map entry is not valid for this map");
    assert((index <= m_mapEntries.size()) && "index to add is out of range") ;
    m_mapEntries.emplace(m_mapEntries.begin() + index, std::make_unique<MapEntry>(std::move(newEntry)));
}

void babelwires::Map::removeMapEntry(unsigned int index) {
    assert((index < m_mapEntries.size()) && "The index to remove is out of range");
    m_mapEntries.erase(m_mapEntries.begin() + index);
}

babelwires::MapData babelwires::Map::extractMapData() const {
    babelwires::MapData mapData;
    mapData.setSourceId(m_sourceId);
    mapData.setTargetId(m_targetId);
    for (const auto& mapEntry : m_mapEntries) {
        mapData.emplaceBack(mapEntry->getData().clone());
    }
    return mapData;
}

void babelwires::Map::setMapData(const MapData& data) {
    setSourceId(data.getSourceId());
    setTargetId(data.getTargetId());
    m_mapEntries.clear();
    for (const auto& mapEntryData : data.m_mapEntries) {
        const std::string reasonForFailure = MapData::validateEntryData(data.getSourceId(), data.getTargetId(), *mapEntryData);
        if (reasonForFailure.empty()) {
            m_mapEntries.emplace_back(std::make_unique<MapEntry>(mapEntryData->clone(), reasonForFailure));
        } else {
            m_mapEntries.emplace_back(std::make_unique<MapEntry>(mapEntryData->clone()));
        }
    }
}
