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
    , m_sourceId(IntType::getThisIdentifier())
    , m_targetId(IntType::getThisIdentifier()) {}

babelwires::MapProject::~MapProject() = default;

void babelwires::MapProject::setAllowedSourceIds(const TypeIdSet& typeSet) {
    // TODO Revalidate data
    m_allowedSourceIds = typeSet;
}

void babelwires::MapProject::setAllowedTargetIds(const TypeIdSet& typeSet) {
    // TODO Revalidate data
    m_allowedTargetIds = typeSet;
}

const babelwires::TypeIdSet& babelwires::MapProject::getAllowedSourceIds() const {
    return m_allowedSourceIds;
}

const babelwires::TypeIdSet& babelwires::MapProject::getAllowedTargetIds() const {
    return m_allowedTargetIds;
}

babelwires::LongIdentifier babelwires::MapProject::getSourceTypeId() const {
    return m_sourceId;
}

babelwires::LongIdentifier babelwires::MapProject::getTargetTypeId() const {
    return m_targetId;
}

void babelwires::MapProject::setSourceTypeId(LongIdentifier sourceId) {
    assert(m_allowedSourceIds.empty() || (std::find(m_allowedSourceIds.begin(), m_allowedSourceIds.end(), sourceId) != m_allowedSourceIds.end()));
    m_sourceId = sourceId;
}

void babelwires::MapProject::setTargetTypeId(LongIdentifier targetId) {
    assert(m_allowedTargetIds.empty() || (std::find(m_allowedTargetIds.begin(), m_allowedTargetIds.end(), targetId) != m_allowedTargetIds.end()));
    m_targetId = targetId;
}

const babelwires::Type* babelwires::MapProject::getSourceType() const {
    return m_projectContext.m_typeSystem.getEntryByIdentifier(m_sourceId);
}

const babelwires::Type* babelwires::MapProject::getTargetType() const {
    return m_projectContext.m_typeSystem.getEntryByIdentifier(m_targetId);
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

bool babelwires::MapProject::validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const {
    const Type *const sourceType = getSourceType();
    if (!sourceType) {
        return false;
    }
    const Type *const targetType = getTargetType();
    if (!targetType) {
        return false;
    }
    return MapData::validateEntryData(*sourceType, *targetType, newEntry, isLastEntry).empty();
}

void babelwires::MapProject::addMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index) {
    assert((index < m_mapEntries.size()) && "You cannot add the last entry of a map. It needs to be a fallback entry.");
    assert(validateNewEntry(*newEntry, false) && "The new map entry is not valid for this map");
    assert((index <= m_mapEntries.size()) && "index to add is out of range") ;
    m_mapEntries.emplace(m_mapEntries.begin() + index, std::make_unique<MapProjectEntry>(std::move(newEntry)));
}

void babelwires::MapProject::removeMapEntry(unsigned int index) {
    assert((index != m_mapEntries.size() - 1) && "You cannot remove the last entry of a map. It needs to be a fallback entry");
    assert((index < m_mapEntries.size()) && "The index to remove is out of range");
    m_mapEntries.erase(m_mapEntries.begin() + index);
}

void babelwires::MapProject::replaceMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index) {
    assert((index < m_mapEntries.size()) && "index to replace is out of range");
    assert(validateNewEntry(*newEntry, (index == m_mapEntries.size() - 1)) && "The new map entry is not valid for this map");
    m_mapEntries[index] = std::make_unique<MapProjectEntry>(std::move(newEntry));
}

babelwires::MapData babelwires::MapProject::extractMapData() const {
    babelwires::MapData mapData;
    mapData.setSourceTypeId(m_sourceId);
    mapData.setTargetTypeId(m_targetId);
    for (const auto& mapEntry : m_mapEntries) {
        mapData.emplaceBack(mapEntry->getData().clone());
    }
    return mapData;
}

void babelwires::MapProject::setMapData(const MapData& data) {
    setSourceTypeId(data.getSourceTypeId());
    setTargetTypeId(data.getTargetTypeId());
    m_mapEntries.clear();
    std::string commonReason;
    const Type *const sourceType = getSourceType();
    const Type *const targetType = getTargetType();
    if (!sourceType && !targetType) {
        commonReason = "Neither source nor target type are known";
    } else if (!sourceType) {
        commonReason = "The source type is unknown";
    } else if (!targetType) {
        commonReason = "The target type is unknown";
    }
    for (unsigned int i = 0; i < data.m_mapEntries.size(); ++i) {
        const auto& mapEntryData = data.m_mapEntries[i];
        const std::string reasonForFailure = MapData::validateEntryData(*sourceType, *targetType, *mapEntryData, (i == data.m_mapEntries.size() - 1));
        if (reasonForFailure.empty()) {
            m_mapEntries.emplace_back(std::make_unique<MapProjectEntry>(mapEntryData->clone(), reasonForFailure));
        } else {
            m_mapEntries.emplace_back(std::make_unique<MapProjectEntry>(mapEntryData->clone()));
        }
    }
}

const babelwires::ProjectContext& babelwires::MapProject::getProjectContext() const {
    return m_projectContext;
}

/// Convenience method which returns the first allowed source type id or int.
babelwires::LongIdentifier babelwires::MapProject::getDefaultSourceId() const {
    if (m_allowedSourceIds.empty()) {
        return IntType::getThisIdentifier();
    } else {
        return m_allowedSourceIds[0];
    }
}

/// Convenience method which returns the first allowed target type id or int.
babelwires::LongIdentifier babelwires::MapProject::getDefaultTargetId() const {
    if (m_allowedTargetIds.empty()) {
        return IntType::getThisIdentifier();
    } else {
        return m_allowedTargetIds[0];
    }
}
