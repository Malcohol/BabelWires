/**
 * The MapProject carries an enriched version of the data in a MapData and has support for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/mapProject.hpp>

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Maps/mapProjectEntry.hpp>
#include <BabelWiresLib/Maps/typeSystem.hpp>

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

babelwires::MapProject::MapProject(const ProjectContext& ProjectContext)
    : m_projectContext(ProjectContext)
    , m_sourceId(TypeSystem::getBuiltInTypeId(TypeSystem::Kind::Int))
    , m_targetId(TypeSystem::getBuiltInTypeId(TypeSystem::Kind::Int)) {}

babelwires::MapProject::~MapProject() = default;

babelwires::LongIdentifier babelwires::MapProject::getSourceId() const {
    return m_sourceId;
}

babelwires::LongIdentifier babelwires::MapProject::getTargetId() const {
    return m_targetId;
}

void babelwires::MapProject::setSourceId(LongIdentifier sourceId) {
    m_sourceId = sourceId;
}

void babelwires::MapProject::setTargetId(LongIdentifier targetId) {
    m_targetId = targetId;
}

bool babelwires::MapProject::operator==(const MapProject& other) const {
    if ((m_sourceId != other.m_sourceId) || (m_targetId != other.m_targetId)) {
        return false;
    }
    return m_mapEntries == other.m_mapEntries;
}

bool babelwires::MapProject::operator!=(const MapProject& other) const {
    if ((m_sourceId != other.m_sourceId) || (m_targetId != other.m_targetId)) {
        return true;
    }
    return m_mapEntries != other.m_mapEntries;
}

unsigned int babelwires::MapProject::getNumMapEntries() const {
    return m_mapEntries.size();
}

const babelwires::MapProjectEntry& babelwires::MapProject::getMapEntry(unsigned int index) const {
    return *m_mapEntries[index];
}

bool babelwires::MapProject::validateNewEntry(const MapEntryData& newEntry) const {
    return MapData::validateEntryData(m_projectContext, m_sourceId, m_targetId, newEntry).empty();
}

void babelwires::MapProject::addMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index) {
    assert(validateNewEntry(*newEntry) && "The new map entry is not valid for this map");
    assert((index <= m_mapEntries.size()) && "index to add is out of range") ;
    m_mapEntries.emplace(m_mapEntries.begin() + index, std::make_unique<MapProjectEntry>(std::move(newEntry)));
}

void babelwires::MapProject::removeMapEntry(unsigned int index) {
    assert((index < m_mapEntries.size()) && "The index to remove is out of range");
    m_mapEntries.erase(m_mapEntries.begin() + index);
}

babelwires::MapData babelwires::MapProject::extractMapData() const {
    babelwires::MapData mapData;
    mapData.setSourceId(m_sourceId);
    mapData.setTargetId(m_targetId);
    for (const auto& mapEntry : m_mapEntries) {
        mapData.emplaceBack(mapEntry->getData().clone());
    }
    return mapData;
}

void babelwires::MapProject::setMapData(const MapData& data) {
    setSourceId(data.getSourceId());
    setTargetId(data.getTargetId());
    m_mapEntries.clear();
    for (const auto& mapEntryData : data.m_mapEntries) {
        const std::string reasonForFailure = MapData::validateEntryData(m_projectContext, data.getSourceId(), data.getTargetId(), *mapEntryData);
        if (reasonForFailure.empty()) {
            m_mapEntries.emplace_back(std::make_unique<MapProjectEntry>(mapEntryData->clone(), reasonForFailure));
        } else {
            m_mapEntries.emplace_back(std::make_unique<MapProjectEntry>(mapEntryData->clone()));
        }
    }
}
