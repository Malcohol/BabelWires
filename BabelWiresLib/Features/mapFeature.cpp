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
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

std::string babelwires::MapFeature::doGetValueType() const {
    return "map";
}

babelwires::MapFeature::MapFeature(LongIdentifier sourceTypeId, LongIdentifier targetTargetId) 
    : m_sourceTypeId(std::move(sourceTypeId))
    , m_targetTypeId(std::move(targetTargetId))
{
}

void babelwires::MapFeature::onBeforeSetValue(const MapData& newValue) const {
    const LongIdentifier& newSourceType = newValue.getSourceTypeId();
    const LongIdentifier& newTargetType = newValue.getTargetTypeId();

    const ProjectContext& context = babelwires::RootFeature::getProjectContextAt(*this);
    const TypeSystem& typeSystem = context.m_typeSystem;

    const bool contravariance = typeSystem.isSubType(m_sourceTypeId, newSourceType);
    const bool covariance = typeSystem.isSubType(newTargetType, m_targetTypeId);
    if (!contravariance && !covariance) {
        throw ModelException() << "Neither the source nor the target types of the map are valid for this feature";
    } else if (!contravariance) {
        throw ModelException() << "The source type of the map is not valid for this feature";
    } else if (!covariance) {
        throw ModelException() << "The target type of the map is not valid for this feature";
    }

    if (!newValue.isValid(context)) {
        throw ModelException() << "The map is not valid";
    }
}

babelwires::LongIdentifier babelwires::MapFeature::getSourceTypeId() const {
    return m_sourceTypeId;
}

babelwires::LongIdentifier babelwires::MapFeature::getTargetTypeId() const {
    return m_targetTypeId;
}

void babelwires::MapFeature::doSetToDefault() {
    MapData mapData;
    mapData.setSourceTypeId(m_sourceTypeId);
    mapData.setTargetTypeId(m_targetTypeId);
    mapData.setEntriesToDefault(RootFeature::getProjectContextAt(*this));
    set(std::move(mapData));
}
