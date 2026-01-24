/**
 * The MapProject carries an enriched version of the data in a MapValue and has support for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/MapProject/mapProject.hpp>

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectEntry.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

babelwires::MapProject::MapProject(const ProjectContext& projectContext)
    : m_projectContext(projectContext)
    , m_sourceTypeValidity(Result::Success())
    , m_targetTypeValidity(Result::Success()) {}

babelwires::MapProject::~MapProject() = default;

void babelwires::MapProject::setAllowedSourceTypeExps(const AllowedTypes& allowedTypes) {
    m_allowedSourceTypeExps = allowedTypes;
    setCurrentSourceTypeExp(allowedTypes.getDefaultTypeExp());
}

void babelwires::MapProject::setAllowedTargetTypeExps(const AllowedTypes& allowedTypes) {
    m_allowedTargetTypeExps = allowedTypes;
    setCurrentTargetTypeExp(allowedTypes.getDefaultTypeExp());
}

const babelwires::MapProject::AllowedTypes& babelwires::MapProject::getAllowedSourceTypeExps() const {
    return m_allowedSourceTypeExps;
}

const babelwires::MapProject::AllowedTypes& babelwires::MapProject::getAllowedTargetTypeExps() const {
    return m_allowedTargetTypeExps;
}

const babelwires::TypeExp& babelwires::MapProject::getCurrentSourceTypeExp() const {
    return m_currentSourceTypeExp;
}

const babelwires::TypeExp& babelwires::MapProject::getCurrentTargetTypeExp() const {
    return m_currentTargetTypeExp;
}

void babelwires::MapProject::setCurrentSourceTypeExp(const TypeExp& sourceId) {
    const TypeSystem& typeSystem = m_projectContext.m_typeSystem;
    TypePtr type = sourceId.tryResolve(typeSystem);
    if (!type) {
        // TODO Add type name.
        m_sourceTypeValidity = "The source type is not recognized.";
        m_currentSourceType = nullptr;
    } else if (!m_allowedSourceTypeExps.isRelatedToSome(typeSystem, sourceId)) {
        // TODO Add type name.
        m_sourceTypeValidity = "The source type is not valid here.";
        m_currentSourceType = nullptr;
    } else {
        m_sourceTypeValidity = Result::Success();
        m_currentSourceType = type;
    }

    m_currentSourceTypeExp = sourceId;

    for (std::size_t i = 0; i < m_mapEntries.size(); ++i) {
        m_mapEntries[i]->validate(typeSystem, m_currentSourceType, m_currentTargetType, (i == m_mapEntries.size() - 1));
    }
}

void babelwires::MapProject::setCurrentTargetTypeExp(const TypeExp& targetId) {
    const TypeSystem& typeSystem = m_projectContext.m_typeSystem;
    TypePtr type = targetId.tryResolve(typeSystem);
    if (!type) {
        // TODO Add type name.
        m_targetTypeValidity = "The target type is not recognized.";
        m_currentTargetType = nullptr;
    } else if (!m_allowedTargetTypeExps.isSubtypeOfSome(typeSystem, targetId)) {
        // TODO Add type name.
        m_targetTypeValidity = "The target type is not valid here.";
        m_currentTargetType = nullptr;
    } else {
        m_targetTypeValidity = Result::Success();
        m_currentTargetType = type;
    }

    m_currentTargetTypeExp = targetId;

    for (std::size_t i = 0; i < m_mapEntries.size(); ++i) {
        m_mapEntries[i]->validate(typeSystem, m_currentSourceType, m_currentTargetType, (i == m_mapEntries.size() - 1));
    }
}

const babelwires::TypePtr& babelwires::MapProject::getCurrentSourceType() const {
    return m_currentSourceType;
}

const babelwires::TypePtr& babelwires::MapProject::getCurrentTargetType() const {
    return m_currentTargetType;
}

unsigned int babelwires::MapProject::getNumMapEntries() const {
    return m_mapEntries.size();
}

const babelwires::MapProjectEntry& babelwires::MapProject::getMapEntry(unsigned int index) const {
    return *m_mapEntries[index];
}

babelwires::Result babelwires::MapProject::validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const {
    return newEntry.validate(m_projectContext.m_typeSystem, *m_currentSourceType, *m_currentTargetType, isLastEntry);
}

void babelwires::MapProject::addMapEntry(std::unique_ptr<MapEntryData> newEntryData, unsigned int index) {
    assert((index < m_mapEntries.size()) && "You cannot add the last entry of a map. It needs to be a fallback entry.");
    assert((index <= m_mapEntries.size()) && "index to add is out of range");
    auto newEntry = std::make_unique<MapProjectEntry>(std::move(newEntryData));
    newEntry->validate(m_projectContext.m_typeSystem, m_currentSourceType, m_currentTargetType, false);
    m_mapEntries.emplace(m_mapEntries.begin() + index, std::move(newEntry));
}

void babelwires::MapProject::removeMapEntry(unsigned int index) {
    assert((index != m_mapEntries.size() - 1) &&
           "You cannot remove the last entry of a map. It needs to be a fallback entry");
    assert((index < m_mapEntries.size()) && "The index to remove is out of range");
    m_mapEntries.erase(m_mapEntries.begin() + index);
}

void babelwires::MapProject::replaceMapEntry(std::unique_ptr<MapEntryData> newEntryData, unsigned int index) {
    assert((index < m_mapEntries.size()) && "index to replace is out of range");
    const bool isLastEntry = (index == m_mapEntries.size() - 1);
    auto newEntry = std::make_unique<MapProjectEntry>(std::move(newEntryData));
    newEntry->validate(m_projectContext.m_typeSystem, m_currentSourceType, m_currentTargetType, isLastEntry);
    m_mapEntries[index] = std::move(newEntry);
}

babelwires::MapValue babelwires::MapProject::extractMapValue() const {
    babelwires::MapValue mapValue;
    mapValue.setSourceTypeExp(m_currentSourceTypeExp);
    mapValue.setTargetTypeExp(m_currentTargetTypeExp);
    for (const auto& mapEntry : m_mapEntries) {
        mapValue.emplaceBack(mapEntry->getData().clone());
    }
    return mapValue;
}

void babelwires::MapProject::setMapValue(const MapValue& data) {
    setCurrentSourceTypeExp(data.getSourceTypeExp());
    setCurrentTargetTypeExp(data.getTargetTypeExp());
    m_mapEntries.clear();
    for (unsigned int i = 0; i < data.m_mapEntries.size(); ++i) {
        const auto& mapEntryData = data.m_mapEntries[i];
        auto mapEntry = std::make_unique<MapProjectEntry>(mapEntryData->clone());
        const bool isLastEntry = (i == data.m_mapEntries.size() - 1);
        mapEntry->validate(m_projectContext.m_typeSystem, m_currentSourceType, m_currentTargetType, isLastEntry);
        m_mapEntries.emplace_back(std::move(mapEntry));
    }
}

const babelwires::ProjectContext& babelwires::MapProject::getProjectContext() const {
    return m_projectContext;
}

const babelwires::Result& babelwires::MapProject::getSourceTypeValidity() const {
    return m_sourceTypeValidity;
}

const babelwires::Result& babelwires::MapProject::getTargetTypeValidity() const {
    return m_targetTypeValidity;
}


bool babelwires::MapProject::AllowedTypes::isRelatedToSome(const TypeSystem& typeSystem, const TypeExp& typeExp) const {
    const TypePtr type = typeExp.tryResolve(typeSystem);
    if (!type) {
        return false;
    }
    return std::any_of(m_typeExps.begin(), m_typeExps.end(), [type, &typeSystem](const TypeExp& id) {
        const TypePtr idType = id.tryResolve(typeSystem);
        return idType && typeSystem.isRelatedType(*idType, *type);
    });
}

bool babelwires::MapProject::AllowedTypes::isSubtypeOfSome(const TypeSystem& typeSystem, const TypeExp& typeExp) const {
    const TypePtr type = typeExp.tryResolve(typeSystem);
    if (!type) {
        return false;
    }
    return std::any_of(m_typeExps.begin(), m_typeExps.end(),
                                        [type, &typeSystem](const TypeExp& id) {
                                            const TypePtr idType = id.tryResolve(typeSystem);
                                            return idType && typeSystem.isSubType(*type, *idType);
                                        });
}