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

void babelwires::MapFeature::onBeforeSetValue(const MapData& newValue) const {
    const TypeRef& newSourceType = newValue.getSourceTypeId();
    const TypeRef& newTargetType = newValue.getTargetTypeId();

    const ProjectContext& context = babelwires::RootFeature::getProjectContextAt(*this);
    const TypeSystem& typeSystem = context.m_typeSystem;

    AllowedTypes allowedTypes;
    getAllowedSourceTypeIds(allowedTypes);
    const bool relatedSource = allowedTypes.isRelatedToSome(typeSystem, newSourceType);
    getAllowedTargetTypeIds(allowedTypes);
    const bool covariance = allowedTypes.isSubtypeOfSome(typeSystem, newTargetType);

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

babelwires::MapData babelwires::MapFeature::getStandardDefaultMapData(MapEntryData::Kind fallbackKind) const {
    assert(MapEntryData::isFallback(fallbackKind) && "Only a fallback kind is expected here");

    AllowedTypes allowedTypes;
    getAllowedSourceTypeIds(allowedTypes);
    TypeRef defaultSourceTypeId = allowedTypes.getDefaultTypeId();
    getAllowedTargetTypeIds(allowedTypes);
    TypeRef defaultTargetTypeId = allowedTypes.getDefaultTypeId();

    MapData mapData;
    mapData.setSourceTypeId(defaultSourceTypeId);
    mapData.setTargetTypeId(defaultTargetTypeId);

    const TypeSystem& typeSystem = RootFeature::getProjectContextAt(*this).m_typeSystem;
    mapData.emplaceBack(MapEntryData::create(typeSystem, defaultSourceTypeId, defaultTargetTypeId, fallbackKind));
    return mapData;
}

babelwires::MapData babelwires::MapFeature::getDefaultMapData() const {
    return getStandardDefaultMapData(MapEntryData::Kind::AllToOne);
}

void babelwires::MapFeature::doSetToDefault() {
    set(getDefaultMapData());
}

bool babelwires::MapFeature::AllowedTypes::isRelatedToSome(const TypeSystem& typeSystem, const TypeRef& typeId) const {
    return std::any_of(m_typeIds.begin(), m_typeIds.end(), [typeId, &typeSystem](const TypeRef& id) {
        return typeSystem.isRelatedType(id, typeId);
    });
}

bool babelwires::MapFeature::AllowedTypes::isSubtypeOfSome(const TypeSystem& typeSystem, const TypeRef& typeId) const {
    return std::any_of(m_typeIds.begin(), m_typeIds.end(),
                                        [typeId, &typeSystem](const TypeRef& id) {
                                            return typeSystem.isSubType(typeId, id);
                                        });
}

babelwires::StandardMapFeature::StandardMapFeature(TypeRef sourceTypeId, TypeRef targetTargetId)
    : m_allowedSourceTypeId(std::move(sourceTypeId))
    , m_allowedTargetTypeId(std::move(targetTargetId)) {}


void babelwires::StandardMapFeature::getAllowedSourceTypeIds(AllowedTypes& allowedTypesOut) const {
    allowedTypesOut.m_typeIds = {m_allowedSourceTypeId};
    allowedTypesOut.m_indexOfDefault = 0;
}

void babelwires::StandardMapFeature::getAllowedTargetTypeIds(AllowedTypes& allowedTypesOut) const {
    allowedTypesOut.m_typeIds = {m_allowedTargetTypeId};
    allowedTypesOut.m_indexOfDefault = 0;
}