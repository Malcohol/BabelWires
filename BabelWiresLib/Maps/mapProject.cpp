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
#include <BabelWiresLib/TypeSystem/intType.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::MapProject::MapProject(const ProjectContext& ProjectContext)
    : m_projectContext(ProjectContext)
    , m_allowedSourceTypeId(IntType::getThisIdentifier())
    , m_allowedTargetTypeId(IntType::getThisIdentifier())
    , m_sourceTypeId(IntType::getThisIdentifier())
    , m_targetTypeId(IntType::getThisIdentifier()) {}

babelwires::MapProject::~MapProject() = default;

void babelwires::MapProject::setAllowedSourceTypeId(LongIdentifier sourceTypeId) {
    // TODO Revalidate data
    m_allowedSourceTypeId = sourceTypeId;
}

void babelwires::MapProject::setAllowedTargetTypeId(LongIdentifier targetTypeId) {
    // TODO Revalidate data
    m_allowedTargetTypeId = targetTypeId;
}

babelwires::LongIdentifier babelwires::MapProject::getAllowedSourceTypeId() const {
    return m_allowedSourceTypeId;
}

babelwires::LongIdentifier babelwires::MapProject::getAllowedTargetTypeId() const {
    return m_allowedTargetTypeId;
}

babelwires::LongIdentifier babelwires::MapProject::getSourceTypeId() const {
    return m_sourceTypeId;
}

babelwires::LongIdentifier babelwires::MapProject::getTargetTypeId() const {
    return m_targetTypeId;
}

void babelwires::MapProject::setSourceTypeId(LongIdentifier sourceId) {
    const TypeSystem& typeSystem = m_projectContext.m_typeSystem;
    assert(typeSystem.isRelatedType(sourceId, m_allowedSourceTypeId));

    m_sourceTypeId = sourceId;

    for (std::size_t i = 0; i < m_mapEntries.size(); ++i) {
        m_mapEntries[i]->validate(typeSystem, m_sourceTypeId, m_targetTypeId, (i == m_mapEntries.size() - 1));
    }
}

void babelwires::MapProject::setTargetTypeId(LongIdentifier targetId) {
    const TypeSystem& typeSystem = m_projectContext.m_typeSystem;
    assert(typeSystem.isSubType(targetId, m_allowedTargetTypeId));
    
    m_targetTypeId = targetId;

    for (std::size_t i = 0; i < m_mapEntries.size(); ++i) {
        m_mapEntries[i]->validate(typeSystem, m_sourceTypeId, m_targetTypeId, (i == m_mapEntries.size() - 1));
    }
}

const babelwires::Type* babelwires::MapProject::getSourceType() const {
    return m_projectContext.m_typeSystem.getEntryByIdentifier(m_sourceTypeId);
}

const babelwires::Type* babelwires::MapProject::getTargetType() const {
    return m_projectContext.m_typeSystem.getEntryByIdentifier(m_targetTypeId);
}

bool babelwires::MapProject::operator==(const MapProject& other) const {
    if ((m_sourceTypeId != other.m_sourceTypeId) || (m_targetTypeId != other.m_targetTypeId)) {
        return false;
    }
    return m_mapEntries == other.m_mapEntries;
}

bool babelwires::MapProject::operator!=(const MapProject& other) const {
    if ((m_sourceTypeId != other.m_sourceTypeId) || (m_targetTypeId != other.m_targetTypeId)) {
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

babelwires::Result babelwires::MapProject::validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const {
    return newEntry.validate(m_projectContext.m_typeSystem, m_sourceTypeId, m_targetTypeId, isLastEntry);
}

void babelwires::MapProject::addMapEntry(std::unique_ptr<MapEntryData> newEntryData, unsigned int index) {
    assert((index < m_mapEntries.size()) && "You cannot add the last entry of a map. It needs to be a fallback entry.");
    assert((index <= m_mapEntries.size()) && "index to add is out of range") ;
    auto newEntry = std::make_unique<MapProjectEntry>(std::move(newEntryData));
    newEntry->validate(m_projectContext.m_typeSystem, m_sourceTypeId, m_targetTypeId, false);
    m_mapEntries.emplace(m_mapEntries.begin() + index, std::move(newEntry) );
}

void babelwires::MapProject::removeMapEntry(unsigned int index) {
    assert((index != m_mapEntries.size() - 1) && "You cannot remove the last entry of a map. It needs to be a fallback entry");
    assert((index < m_mapEntries.size()) && "The index to remove is out of range");
    m_mapEntries.erase(m_mapEntries.begin() + index);
}

void babelwires::MapProject::replaceMapEntry(std::unique_ptr<MapEntryData> newEntryData, unsigned int index) {
    assert((index < m_mapEntries.size()) && "index to replace is out of range");
    const bool isLastEntry = (index == m_mapEntries.size() - 1);
    auto newEntry = std::make_unique<MapProjectEntry>(std::move(newEntryData));
    newEntry->validate(m_projectContext.m_typeSystem, m_sourceTypeId, m_targetTypeId, isLastEntry);
    m_mapEntries[index] = std::move(newEntry);
}

babelwires::MapData babelwires::MapProject::extractMapData() const {
    babelwires::MapData mapData;
    mapData.setSourceTypeId(m_sourceTypeId);
    mapData.setTargetTypeId(m_targetTypeId);
    for (const auto& mapEntry : m_mapEntries) {
        mapData.emplaceBack(mapEntry->getData().clone());
    }
    return mapData;
}

void babelwires::MapProject::setMapData(const MapData& data) {
    setSourceTypeId(data.getSourceTypeId());
    setTargetTypeId(data.getTargetTypeId());
    m_mapEntries.clear();
    for (unsigned int i = 0; i < data.m_mapEntries.size(); ++i) {
        const auto& mapEntryData = data.m_mapEntries[i];
        auto mapEntry = std::make_unique<MapProjectEntry>(mapEntryData->clone());
        const bool isLastEntry = (i == m_mapEntries.size() - 1);
        mapEntry->validate(m_projectContext.m_typeSystem, m_sourceTypeId, m_targetTypeId, isLastEntry);
        m_mapEntries.emplace_back(std::move(mapEntry));
    }
}

const babelwires::ProjectContext& babelwires::MapProject::getProjectContext() const {
    return m_projectContext;
}
