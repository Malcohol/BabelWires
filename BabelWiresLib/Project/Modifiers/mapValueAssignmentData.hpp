/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Maps/mapData.hpp"

namespace babelwires {
    struct MapValueAssignmentData : LocalModifierData {
        CLONEABLE(MapValueAssignmentData);
        SERIALIZABLE(MapValueAssignmentData, "assignMap", LocalModifierData, 1);
        virtual void apply(Feature* targetFeature) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        MapData m_mapData;
    };
} // namespace babelwires
