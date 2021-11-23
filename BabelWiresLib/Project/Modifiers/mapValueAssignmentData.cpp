/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/Modifiers/mapValueAssignmentData.hpp"

#include "BabelWiresLib/Features/mapFeature.hpp"
#include "BabelWiresLib/Features/modelExceptions.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

void babelwires::MapValueAssignmentData::apply(Feature* targetFeature) const {
    if (MapFeature* mapFeature = targetFeature->as<MapFeature>()) {
        mapFeature->set(m_map);
    } else {
        throw babelwires::ModelException() << "Could not assign a map value to a non-map feature";
    }
}

void babelwires::MapValueAssignmentData::serializeContents(Serializer& serializer) const {
    // inline the contents of m_map.
    m_map.serializeContents(serializer);
}

void babelwires::MapValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    // the contents of m_map are expected to be inlined.
    m_map.deserializeContents(deserializer);
}

void babelwires::MapValueAssignmentData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    visitor(m_map);
}
