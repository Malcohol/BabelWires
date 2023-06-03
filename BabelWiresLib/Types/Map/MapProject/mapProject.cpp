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

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::MapProject::MapProject(const ProjectContext& projectContext)
    : m_projectContext(projectContext)
    , m_sourceTypeValidity(Result::Success())
    , m_targetTypeValidity(Result::Success()) {}

babelwires::MapProject::~MapProject() = default;

void babelwires::MapProject::setAllowedSourceTypeRefs(const AllowedTypes& allowedTypes) {
    m_allowedSourceTypeRefs = allowedTypes;
    setCurrentSourceTypeRef(allowedTypes.getDefaultTypeRef());
}

void babelwires::MapProject::setAllowedTargetTypeRefs(const AllowedTypes& allowedTypes) {
    m_allowedTargetTypeRefs = allowedTypes;
    setCurrentTargetTypeRef(allowedTypes.getDefaultTypeRef());
}

const babelwires::MapProject::AllowedTypes& babelwires::MapProject::getAllowedSourceTypeRefs() const {
    return m_allowedSourceTypeRefs;
}

const babelwires::MapProject::AllowedTypes& babelwires::MapProject::getAllowedTargetTypeRefs() const {
    return m_allowedTargetTypeRefs;
}

const babelwires::TypeRef& babelwires::MapProject::getCurrentSourceTypeRef() const {
    return m_currentSourceTypeRef;
}

const babelwires::TypeRef& babelwires::MapProject::getCurrentTargetTypeRef() const {
    return m_currentTargetTypeRef;
}

void babelwires::MapProject::setCurrentSourceTypeRef(const TypeRef& sourceId) {
    const TypeSystem& typeSystem = m_projectContext.m_typeSystem;
    const Type *const type = sourceId.tryResolve(typeSystem);
    if (!type) {
        // TODO Add type name.
        m_sourceTypeValidity = "The source type is not recognized.";
    } else if (type->isAbstract()) {
        m_sourceTypeValidity = "The source type is abstract.";
    } else if (!m_allowedSourceTypeRefs.isRelatedToSome(typeSystem, sourceId)) {
        // TODO Add type name.
        m_sourceTypeValidity = "The source type is not valid here.";
    } else {
        m_sourceTypeValidity = Result::Success();
    }

    m_currentSourceTypeRef = sourceId;

    for (std::size_t i = 0; i < m_mapEntries.size(); ++i) {
        m_mapEntries[i]->validate(typeSystem, m_currentSourceTypeRef, m_currentTargetTypeRef, (i == m_mapEntries.size() - 1));
    }
}

void babelwires::MapProject::setCurrentTargetTypeRef(const TypeRef& targetId) {
    const TypeSystem& typeSystem = m_projectContext.m_typeSystem;
    const Type *const type = targetId.tryResolve(typeSystem);
    if (!type) {
        // TODO Add type name.
        m_targetTypeValidity = "The target type is not recognized.";
    } else if (type->isAbstract()) {
        m_targetTypeValidity = "The target type is abstract.";
    } else if (!m_allowedTargetTypeRefs.isSubtypeOfSome(typeSystem, targetId)) {
        // TODO Add type name.
        m_targetTypeValidity = "The target type is not valid here.";
    } else {
        m_targetTypeValidity = Result::Success();
    }

    m_currentTargetTypeRef = targetId;

    for (std::size_t i = 0; i < m_mapEntries.size(); ++i) {
        m_mapEntries[i]->validate(typeSystem, m_currentSourceTypeRef, m_currentTargetTypeRef, (i == m_mapEntries.size() - 1));
    }
}

const babelwires::Type* babelwires::MapProject::getCurrentSourceType() const {
    // TODO Could this fail to resolve?
    return m_currentSourceTypeRef.tryResolve(m_projectContext.m_typeSystem);
}

const babelwires::Type* babelwires::MapProject::getCurrentTargetType() const {
    // TODO Could this fail to resolve?
    return m_currentTargetTypeRef.tryResolve(m_projectContext.m_typeSystem);
}

unsigned int babelwires::MapProject::getNumMapEntries() const {
    return m_mapEntries.size();
}

const babelwires::MapProjectEntry& babelwires::MapProject::getMapEntry(unsigned int index) const {
    return *m_mapEntries[index];
}

babelwires::Result babelwires::MapProject::validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const {
    return newEntry.validate(m_projectContext.m_typeSystem, m_currentSourceTypeRef, m_currentTargetTypeRef, isLastEntry);
}

void babelwires::MapProject::addMapEntry(std::unique_ptr<MapEntryData> newEntryData, unsigned int index) {
    assert((index < m_mapEntries.size()) && "You cannot add the last entry of a map. It needs to be a fallback entry.");
    assert((index <= m_mapEntries.size()) && "index to add is out of range");
    auto newEntry = std::make_unique<MapProjectEntry>(std::move(newEntryData));
    newEntry->validate(m_projectContext.m_typeSystem, m_currentSourceTypeRef, m_currentTargetTypeRef, false);
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
    newEntry->validate(m_projectContext.m_typeSystem, m_currentSourceTypeRef, m_currentTargetTypeRef, isLastEntry);
    m_mapEntries[index] = std::move(newEntry);
}

babelwires::MapValue babelwires::MapProject::extractMapValue() const {
    babelwires::MapValue mapValue;
    mapValue.setSourceTypeRef(m_currentSourceTypeRef);
    mapValue.setTargetTypeRef(m_currentTargetTypeRef);
    for (const auto& mapEntry : m_mapEntries) {
        mapValue.emplaceBack(mapEntry->getData().clone());
    }
    return mapValue;
}

void babelwires::MapProject::setMapValue(const MapValue& data) {
    setCurrentSourceTypeRef(data.getSourceTypeRef());
    setCurrentTargetTypeRef(data.getTargetTypeRef());
    m_mapEntries.clear();
    for (unsigned int i = 0; i < data.m_mapEntries.size(); ++i) {
        const auto& mapEntryData = data.m_mapEntries[i];
        auto mapEntry = std::make_unique<MapProjectEntry>(mapEntryData->clone());
        const bool isLastEntry = (i == data.m_mapEntries.size() - 1);
        mapEntry->validate(m_projectContext.m_typeSystem, m_currentSourceTypeRef, m_currentTargetTypeRef, isLastEntry);
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


bool babelwires::MapProject::AllowedTypes::isRelatedToSome(const TypeSystem& typeSystem, const TypeRef& typeRef) const {
    return std::any_of(m_typeRefs.begin(), m_typeRefs.end(), [typeRef, &typeSystem](const TypeRef& id) {
        return typeSystem.isRelatedType(id, typeRef);
    });
}

bool babelwires::MapProject::AllowedTypes::isSubtypeOfSome(const TypeSystem& typeSystem, const TypeRef& typeRef) const {
    return std::any_of(m_typeRefs.begin(), m_typeRefs.end(),
                                        [typeRef, &typeSystem](const TypeRef& id) {
                                            return typeSystem.isSubType(typeRef, id);
                                        });
}