/**
 * A MapData defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/mapData.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
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

babelwires::MapData::MapData(const TypeSystem& typeSystem, TypeRef sourceRef, TypeRef targetRef, MapEntryData::Kind fallbackKind) 
    : m_sourceTypeRef(std::move(sourceRef))
    , m_targetTypeRef(std::move(targetRef))
{
    m_mapEntries.emplace_back(MapEntryData::create(typeSystem, m_sourceTypeRef, m_targetTypeRef, fallbackKind));
}

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

bool babelwires::MapData::operator==(const Value& other) const {
    if (const MapData* otherMap = other.as<MapData>()) {
        return *this == *otherMap;
    }
    return false;
}

bool babelwires::MapData::operator==(const MapData& other) const {
    if ((m_sourceTypeRef != other.m_sourceTypeRef) || (m_targetTypeRef != other.m_targetTypeRef)) {
        return false;
    }
    return std::equal(m_mapEntries.begin(), m_mapEntries.end(), other.m_mapEntries.begin(), other.m_mapEntries.end(),
                      [](const auto& a, const auto& b) { return (*a == *b); });
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

std::string babelwires::MapData::toString() const {
    std::ostringstream os;
    os << getNumMapEntries() << " entries";
    return os.str();
}

bool babelwires::MapData::canContainIdentifiers() const {
    return true;
}
bool babelwires::MapData::canContainFilePaths() const {
    // This is very unlikely, but since it only impacts serialization performance, so let's play it safe.
    return true;
}

const babelwires::MapEntryData& babelwires::MapData::getMapEntry(unsigned int index) const {
    assert(index < m_mapEntries.size() && "Index to getMapEntry out of range");
    return *m_mapEntries[index];
}

bool babelwires::MapData::isValid(const TypeSystem& typeSystem) const {
    for (unsigned int i = 0; i < m_mapEntries.size(); ++i) {
        const auto& entryData = m_mapEntries[i];
        if (!entryData->validate(typeSystem, m_sourceTypeRef, m_targetTypeRef, (i == m_mapEntries.size() - 1))) {
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
    serializer.serializeObject(m_sourceTypeRef, "sourceType");
    serializer.serializeObject(m_targetTypeRef, "targetType");
    serializer.serializeArray("entries", m_mapEntries);
}

void babelwires::MapData::deserializeContents(Deserializer& deserializer) {
    m_sourceTypeRef = std::move(*deserializer.deserializeObject<TypeRef>("sourceType"));
    m_targetTypeRef = std::move(*deserializer.deserializeObject<TypeRef>("targetType"));
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
