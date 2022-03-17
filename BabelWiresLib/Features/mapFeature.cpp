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

babelwires::MapFeature::MapFeature(TypeSet allowedSourceIds, TypeSet allowedTargetIds) 
    : m_allowedSourceIds(std::move(allowedSourceIds))
    , m_allowedTargetIds(std::move(allowedTargetIds))
{
}

void babelwires::MapFeature::onBeforeSetValue(const MapData& newValue) const {
    const LongIdentifier& newSourceType = newValue.getSourceId();
    const LongIdentifier& newTargetType = newValue.getTargetId();

    if (!m_allowedSourceIds.empty() && (std::find(m_allowedSourceIds.begin(), m_allowedSourceIds.end(), newSourceType) == m_allowedSourceIds.end())) {
        throw ModelException() << "The type \"" << IdentifierRegistry::read()->getName(newSourceType) << "\" is not a permitted source type for this map feature";
    }
    if (!m_allowedTargetIds.empty() && (std::find(m_allowedTargetIds.begin(), m_allowedTargetIds.end(), newTargetType) == m_allowedTargetIds.end())) {
        throw ModelException() << "The type \"" << IdentifierRegistry::read()->getName(newTargetType) << "\" is not a permitted target type for this map feature";
    }
    if (newValue.hasInvalidEntries(babelwires::RootFeature::getProjectContextAt(*this))) {
        throw ModelException() << "The map has invalid entries";
    }
}

const babelwires::MapFeature::TypeSet& babelwires::MapFeature::getAllowedSourceIds() const {
    return m_allowedSourceIds;
}

const babelwires::MapFeature::TypeSet& babelwires::MapFeature::getAllowedTargetIds() const {
    return m_allowedTargetIds;
}

void babelwires::MapFeature::doSetToDefault() {
    MapData mapData;
    if (!m_allowedSourceIds.empty()) {
        mapData.setSourceId(m_allowedSourceIds[0]);
    }
    if (!m_allowedTargetIds.empty()) {
        mapData.setTargetId(m_allowedTargetIds[0]);
    }
    set(std::move(mapData));
}
