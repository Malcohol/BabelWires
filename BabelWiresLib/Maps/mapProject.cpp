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
    , m_sourceTypeId(IntType::getThisIdentifier())
    , m_targetTypeId(IntType::getThisIdentifier()) {}

babelwires::MapProject::~MapProject() = default;

void babelwires::MapProject::setAllowedSourceTypeIds(const TypeIdSet& typeSet) {
    // TODO Revalidate data
    m_allowedSourceTypeIds = typeSet;
}

void babelwires::MapProject::setAllowedTargetTypeIds(const TypeIdSet& typeSet) {
    // TODO Revalidate data
    m_allowedTargetTypeIds = typeSet;
}

const babelwires::TypeIdSet& babelwires::MapProject::getAllowedSourceTypeIds() const {
    return m_allowedSourceTypeIds;
}

const babelwires::TypeIdSet& babelwires::MapProject::getAllowedTargetTypeIds() const {
    return m_allowedTargetTypeIds;
}

babelwires::LongIdentifier babelwires::MapProject::getSourceTypeId() const {
    return m_sourceTypeId;
}

babelwires::LongIdentifier babelwires::MapProject::getTargetTypeId() const {
    return m_targetTypeId;
}

void babelwires::MapProject::setSourceTypeId(LongIdentifier sourceId) {
    assert(m_allowedSourceTypeIds.empty() || (std::find(m_allowedSourceTypeIds.begin(), m_allowedSourceTypeIds.end(), sourceId) != m_allowedSourceTypeIds.end()));
    m_sourceTypeId = sourceId;
}

void babelwires::MapProject::setTargetTypeId(LongIdentifier targetId) {
    assert(m_allowedTargetTypeIds.empty() || (std::find(m_allowedTargetTypeIds.begin(), m_allowedTargetTypeIds.end(), targetId) != m_allowedTargetTypeIds.end()));
    m_targetTypeId = targetId;
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
    if (m_allowedSourceTypeIds.empty()) {
        return IntType::getThisIdentifier();
    } else {
        return m_allowedSourceTypeIds[0];
    }
}

/// Convenience method which returns the first allowed target type id or int.
babelwires::LongIdentifier babelwires::MapProject::getDefaultTargetId() const {
    if (m_allowedTargetTypeIds.empty()) {
        return IntType::getThisIdentifier();
    } else {
        return m_allowedTargetTypeIds[0];
    }
}
