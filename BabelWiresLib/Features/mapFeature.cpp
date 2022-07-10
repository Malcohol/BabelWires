/**
 * A MapFeature allows the user to define a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/mapFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

std::string babelwires::MapFeature::doGetValueType() const {
    return "map";
}

babelwires::MapFeature::MapFeature(LongIdentifier sourceTypeId, LongIdentifier targetTargetId)
    : m_sourceTypeId(std::move(sourceTypeId))
    , m_targetTypeId(std::move(targetTargetId)) {}

void babelwires::MapFeature::onBeforeSetValue(const MapData& newValue) const {
    const LongIdentifier& newSourceType = newValue.getSourceTypeId();
    const LongIdentifier& newTargetType = newValue.getTargetTypeId();

    const ProjectContext& context = babelwires::RootFeature::getProjectContextAt(*this);
    const TypeSystem& typeSystem = context.m_typeSystem;

    const bool relatedSource = typeSystem.isRelatedType(m_sourceTypeId, newSourceType);
    const bool covariance = typeSystem.isSubType(newTargetType, m_targetTypeId);
    if (!relatedSource && !covariance) {
        throw ModelException() << "Neither the source nor the target types of the map are valid for this feature";
    } else if (!relatedSource) {
        throw ModelException() << "The source type of the map is not valid for this feature";
    } else if (!covariance) {
        throw ModelException() << "The target type of the map is not valid for this feature";
    }

    if (!newValue.isValid(context.m_typeSystem)) {
        throw ModelException() << "The map is not valid";
    }
}

babelwires::LongIdentifier babelwires::MapFeature::getSourceTypeId() const {
    return m_sourceTypeId;
}

babelwires::LongIdentifier babelwires::MapFeature::getTargetTypeId() const {
    return m_targetTypeId;
}

babelwires::MapData babelwires::MapFeature::getStandardDefaultMapData(MapEntryData::Kind fallbackKind) const {
    assert(MapEntryData::isFallback(fallbackKind) && "Only a fallback kind is expected here");

    MapData mapData;
    mapData.setSourceTypeId(m_sourceTypeId);
    mapData.setTargetTypeId(m_targetTypeId);
    const TypeSystem& typeSystem = RootFeature::getProjectContextAt(*this).m_typeSystem;
    mapData.emplaceBack(MapEntryData::create(typeSystem, m_sourceTypeId, m_targetTypeId, fallbackKind));
    return mapData;
}

babelwires::MapData babelwires::MapFeature::getDefaultMapData() const {
    return getStandardDefaultMapData(MapEntryData::Kind::AllToOne);
}

void babelwires::MapFeature::doSetToDefault() {
    set(getDefaultMapData());
}
