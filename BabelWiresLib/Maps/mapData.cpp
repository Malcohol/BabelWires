/**
 * A MapData defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapData.hpp>

#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/intType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::MapData::MapData()
    : m_sourceId(IntType::getThisIdentifier())
    , m_targetId(IntType::getThisIdentifier()) {}

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

babelwires::LongIdentifier babelwires::MapData::getSourceTypeId() const {
    return m_sourceId;
}

babelwires::LongIdentifier babelwires::MapData::getTargetTypeId() const {
    return m_targetId;
}

void babelwires::MapData::setSourceTypeId(LongIdentifier sourceId) {
    m_sourceId = sourceId;
}

void babelwires::MapData::setTargetTypeId(LongIdentifier targetId) {
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

std::string babelwires::MapData::validateEntryData(const Type& sourceType, const Type& targetType,
                                                   const MapEntryData& entryData, bool isLastEntry) {
    if (isLastEntry != (entryData.as<FallbackMapEntryData>() != nullptr)) {
        return isLastEntry ? "The last entry must be a fallback entry" : "A fallback entry can only be at the end of a map";
    }
    if (!entryData.isValid(sourceType, targetType)) {
        return "The entry is invalid";
    }
    return {};
}

bool babelwires::MapData::isValid(const ProjectContext& context) const {
    const Type* sourceType = context.m_typeSystem.getEntryByIdentifier(m_sourceId);
    if (!sourceType) {
        return false;
    }
    const Type* targetType = context.m_typeSystem.getEntryByIdentifier(m_sourceId);
    if (!targetType) {
        return false;
    }
    for (unsigned int i = 0; i < m_mapEntries.size(); ++i) {
        const auto& entryData = m_mapEntries[i];
        if (!validateEntryData(*sourceType, *targetType, *entryData, (i == m_mapEntries.size() - 1)).empty()) {
            return false;
        }
    }
    return true;
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

void babelwires::MapData::setEntriesToDefault(const ProjectContext& context) {
    m_mapEntries.clear();
    const Type* targetType = context.m_typeSystem.getEntryByIdentifier(m_targetId);
    if (targetType) {
        auto fallback = std::make_unique<AllToOneFallbackMapEntryData>();
        fallback->setTargetValue(targetType->createValue());
        m_mapEntries.emplace_back(std::move(fallback));
    }
}
