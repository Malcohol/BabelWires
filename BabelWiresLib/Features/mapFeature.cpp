/**
 * A MapFeature allows the user to define a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Features/mapFeature.hpp"

#include "BabelWiresLib/Features/modelExceptions.hpp"
#include "BabelWiresLib/Features/rootFeature.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"

std::string babelwires::MapFeature::doGetValueType() const {
    return "map";
}

babelwires::MapFeature::MapFeature(TypeIdSet allowedSourceIds, TypeIdSet allowedTargetIds) 
    : m_allowedSourceTypeIds(std::move(allowedSourceIds))
    , m_allowedTargetTypeIds(std::move(allowedTargetIds))
{
}

void babelwires::MapFeature::onBeforeSetValue(const MapData& newValue) const {
    const LongIdentifier& newSourceType = newValue.getSourceTypeId();
    const LongIdentifier& newTargetType = newValue.getTargetTypeId();

    if (!m_allowedSourceTypeIds.empty() && (std::find(m_allowedSourceTypeIds.begin(), m_allowedSourceTypeIds.end(), newSourceType) == m_allowedSourceTypeIds.end())) {
        throw ModelException() << "The type \"" << IdentifierRegistry::read()->getName(newSourceType) << "\" is not a permitted source type for this map feature";
    }
    if (!m_allowedTargetTypeIds.empty() && (std::find(m_allowedTargetTypeIds.begin(), m_allowedTargetTypeIds.end(), newTargetType) == m_allowedTargetTypeIds.end())) {
        throw ModelException() << "The type \"" << IdentifierRegistry::read()->getName(newTargetType) << "\" is not a permitted target type for this map feature";
    }
    if (!newValue.isValid(babelwires::RootFeature::getProjectContextAt(*this))) {
        throw ModelException() << "The map is not valid";
    }
}

const babelwires::TypeIdSet& babelwires::MapFeature::getAllowedSourceTypeIds() const {
    return m_allowedSourceTypeIds;
}

const babelwires::TypeIdSet& babelwires::MapFeature::getAllowedTargetTypeIds() const {
    return m_allowedTargetTypeIds;
}

void babelwires::MapFeature::doSetToDefault() {
    MapData mapData;
    if (!m_allowedSourceTypeIds.empty()) {
        mapData.setSourceTypeId(m_allowedSourceTypeIds[0]);
    }
    if (!m_allowedTargetTypeIds.empty()) {
        mapData.setTargetTypeId(m_allowedTargetTypeIds[0]);
    }
    mapData.setEntriesToDefault(RootFeature::getProjectContextAt(*this));
    set(std::move(mapData));
}
