/**
 * A MapData defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapData.hpp>

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::MapData::MapData() = default;

babelwires::MapData::MapData(const MapData& other)
    : m_sourceTypeRef(other.m_sourceTypeRef)
    , m_targetTypeRef(other.m_targetTypeRef) {
    for (const auto& e : other.m_mapEntries) {
        m_mapEntries.emplace_back(e->clone());
    }
}

babelwires::MapData::MapData(MapData&& other)
    : m_sourceTypeRef(other.m_sourceTypeRef)
    , m_targetTypeRef(other.m_targetTypeRef)
    , m_mapEntries(std::move(other.m_mapEntries)) {}

babelwires::MapData& babelwires::MapData::operator=(const MapData& other) {
    m_sourceTypeRef = other.m_sourceTypeRef;
    m_targetTypeRef = other.m_targetTypeRef;
    for (const auto& e : other.m_mapEntries) {
        m_mapEntries.emplace_back(e->clone());
    }
    return *this;
}

babelwires::MapData& babelwires::MapData::operator=(MapData&& other) {
    m_sourceTypeRef = other.m_sourceTypeRef;
    m_targetTypeRef = other.m_targetTypeRef;
    m_mapEntries = std::move(other.m_mapEntries);
    return *this;
}

babelwires::MapData::~MapData() = default;

const babelwires::TypeRef& babelwires::MapData::getSourceTypeRef() const {
    return m_sourceTypeRef;
}

const babelwires::TypeRef& babelwires::MapData::getTargetTypeRef() const {
    return m_targetTypeRef;
}

void babelwires::MapData::setSourceTypeRef(const TypeRef& sourceId) {
    m_sourceTypeRef = sourceId;
}

void babelwires::MapData::setTargetTypeRef(const TypeRef& targetId) {
    m_targetTypeRef = targetId;
}

bool babelwires::MapData::operator==(const MapData& other) const {
    if ((m_sourceTypeRef != other.m_sourceTypeRef) || (m_targetTypeRef != other.m_targetTypeRef)) {
        return false;
    }
    return std::equal(m_mapEntries.begin(), m_mapEntries.end(), other.m_mapEntries.begin(), other.m_mapEntries.end(),
               [](const auto& a, const auto& b) { return (*a == *b); });
}

bool babelwires::MapData::operator!=(const MapData& other) const {
    return !(*this == other);
}

std::size_t babelwires::MapData::getHash() const {
    std::size_t h = hash::mixtureOf(m_sourceTypeRef, m_targetTypeRef);
    for (const auto& e : m_mapEntries) {
        hash::mixInto(h, *e);
    }
    return h;
}

unsigned int babelwires::MapData::getNumMapEntries() const {
    return m_mapEntries.size();
}

const babelwires::MapEntryData& babelwires::MapData::getMapEntry(unsigned int index) const {
    assert(index < m_mapEntries.size() && "Index to getMapEntry out of range");
    return *m_mapEntries[index];
}

bool babelwires::MapData::isValid(const TypeSystem& typeSystem) const {
    for (unsigned int i = 0; i < m_mapEntries.size(); ++i) {
        const auto& entryData = m_mapEntries[i];
        if (!entryData->validate(typeSystem, m_sourceTypeRef, m_targetTypeRef,
                                 (i == m_mapEntries.size() - 1))) {
            return false;
        }
    }
    return true;
}

void babelwires::MapData::emplaceBack(std::unique_ptr<MapEntryData> newEntry) {
    assert((newEntry != nullptr) && "Null entry added to map");
    m_mapEntries.emplace_back(std::move(newEntry));
}

void babelwires::MapData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("sourceType", m_sourceTypeRef);
    serializer.serializeValue("targetType", m_targetTypeRef);
    serializer.serializeArray("entries", m_mapEntries);
}

void babelwires::MapData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("sourceType", m_sourceTypeRef);
    deserializer.deserializeValue("targetType", m_targetTypeRef);
    auto it = deserializer.deserializeArray<MapEntryData>("entries", Deserializer::IsOptional::Optional);
    while (it.isValid()) {
        std::unique_ptr<MapEntryData> newEntry = it.getObject();
        m_mapEntries.emplace_back(std::move(newEntry));
        ++it;
    }
}

void babelwires::MapData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_sourceTypeRef.visitIdentifiers(visitor);
    m_targetTypeRef.visitIdentifiers(visitor);
    for (const auto& e : m_mapEntries) {
        e->visitIdentifiers(visitor);
    }
}

void babelwires::MapData::visitFilePaths(FilePathVisitor& visitor) {
    for (const auto& e : m_mapEntries) {
        e->visitFilePaths(visitor);
    }
}

void babelwires::MapData::setEntriesToDefault(const TypeSystem& typeSystem) {
    m_mapEntries.clear();
}
