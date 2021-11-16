/**
 * A MapFeature allows the user to define a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Features/mapFeature.hpp"
#include "BabelWiresLib/Features/modelExceptions.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"

std::string babelwires::MapFeature::doGetValueType() const {
    return "map";
}

void babelwires::MapFeature::onBeforeSetValue(const Map& newValue) const {
    const LongIdentifier& newSourceType = newValue.getSourceType();
    const LongIdentifier& newTargetType = newValue.getTargetType();

    if (!m_allowedSourceTypes.empty() && (m_allowedSourceTypes.find(newSourceType) == m_allowedSourceTypes.end())) {
        throw ModelException() << "The type \"" << IdentifierRegistry::read()->getName(newSourceType) << "\" is not a permitted source type for this map feature";
    }
    if (!m_allowedTargetTypes.empty() && (m_allowedTargetTypes.find(newTargetType) == m_allowedTargetTypes.end())) {
        throw ModelException() << "The type \"" << IdentifierRegistry::read()->getName(newTargetType) << "\" is not a permitted target type for this map feature";
    }
}
