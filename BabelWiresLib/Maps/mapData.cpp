/**
 * A MapData defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapData.hpp>

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Maps/typeSystem.hpp>

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

babelwires::MapData::MapData()
    : m_sourceId(getBuiltInTypeId(KnownType::Int))
    , m_targetId(getBuiltInTypeId(KnownType::Int)) {}

babelwires::MapData::MapData(const MapData& other)
    : m_sourceId(other.m_sourceId)
    , m_targetId(other.m_targetId) {
    for (const auto& e : other.m_mapEntries) {
        m_mapEntries.emplace_back(e->clone());
    }
}

babelwires::MapData::MapData(MapData&& other)
    : m_sourceId(other.m_sourceId)
    , m_targetId(other.m_targetId)
    , m_mapEntries(std::move(other.m_mapEntries)) {}

babelwires::MapData& babelwires::MapData::operator=(const MapData& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    for (const auto& e : other.m_mapEntries) {
        m_mapEntries.emplace_back(e->clone());
    }
    return *this;
}

babelwires::MapData& babelwires::MapData::operator=(MapData&& other) {
    m_sourceId = other.m_sourceId;
    m_targetId = other.m_targetId;
    m_mapEntries = std::move(other.m_mapEntries);
    return *this;
}

babelwires::MapData::~MapData() = default;

babelwires::LongIdentifier babelwires::MapData::getSourceId() const {
    return m_sourceId;
}

babelwires::LongIdentifier babelwires::MapData::getTargetId() const {
    return m_targetId;
}

void babelwires::MapData::setSourceId(LongIdentifier sourceId) {
    m_sourceId = sourceId;
}

void babelwires::MapData::setTargetId(LongIdentifier targetId) {
    m_targetId = targetId;
}

bool babelwires::MapData::operator==(const MapData& other) const {
    if ((m_sourceId != other.m_sourceId) || (m_targetId != other.m_targetId)) {
        return false;
    }
    return m_mapEntries == other.m_mapEntries;
}

bool babelwires::MapData::operator!=(const MapData& other) const {
    if ((m_sourceId != other.m_sourceId) || (m_targetId != other.m_targetId)) {
        return true;
    }
    return m_mapEntries != other.m_mapEntries;
}

std::size_t babelwires::MapData::getHash() const {
    std::size_t h = hash::mixtureOf(m_sourceId, m_targetId);
    for (const auto& e : m_mapEntries) {
        hash::mixInto(h, *e);
    }
    return h;
}

unsigned int babelwires::MapData::getNumMapEntries() const {
    return m_mapEntries.size();
}

const babelwires::MapEntryData& babelwires::MapData::getMapEntry(unsigned int index) const {
    return *m_mapEntries[index];
}

std::string babelwires::MapData::validateEntryData(LongIdentifier sourceId, LongIdentifier targetId, const MapEntryData& entryData) {
    if (getTypeFromIdentifier(sourceId) != entryData.getSourceType()) {
        return "The source type does not match";
    }
    if (getTypeFromIdentifier(targetId) != entryData.getTargetType()) {
        return "The target type does not match";
    }
    return {};
}

bool babelwires::MapData::hasInvalidEntries() const {
    for (const auto& entry : m_mapEntries ) {
        if (!validateEntryData(m_sourceId, m_targetId, *entry).empty()) {
            return true;
        }
    }
    return false;
}

void babelwires::MapData::emplaceBack(std::unique_ptr<MapEntryData> newEntry) {
    m_mapEntries.emplace_back(std::move(newEntry));
}

void babelwires::MapData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("sourceId", m_sourceId);
    serializer.serializeValue("targetId", m_targetId);
    serializer.serializeArray("entries", m_mapEntries);
}

void babelwires::MapData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("sourceId", m_sourceId);
    deserializer.deserializeValue("targetId", m_targetId);
    auto it = deserializer.deserializeArray<MapEntryData>("entries", Deserializer::IsOptional::Optional);
    while (it.isValid()) {
        std::unique_ptr<MapEntryData> newEntry = it.getObject();
        m_mapEntries.emplace_back(std::move(newEntry));
        ++it;
    }
}

void babelwires::MapData::visitIdentifiers(IdentifierVisitor& visitor) {
    visitor(m_sourceId);
    visitor(m_targetId);
    for (const auto& e : m_mapEntries) {
        e->visitIdentifiers(visitor);
    }
}

void babelwires::MapData::visitFilePaths(FilePathVisitor& visitor) {
    for (const auto& e : m_mapEntries) {
        e->visitFilePaths(visitor);
    }
}
