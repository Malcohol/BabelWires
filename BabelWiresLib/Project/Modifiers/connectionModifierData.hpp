/**
 * ConnectionModifierData used to assign a ValueFeature within a container to a value from another element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"

namespace babelwires {
    /// Data used to assign a ValueFeature within a container to a value from another element.
    struct ConnectionModifierData : ModifierData {
        /// Find the source feature in the project, or throw.
        const Feature* getSourceFeature(const Project& project) const;

        /// Apply the source value to the target feature.
        void apply(const Feature* sourceFeature, Feature* targetFeature, bool applyEvenIfSourceUnchanged) const;

        virtual std::unique_ptr<Modifier> createModifier() const;

        CLONEABLE(ConnectionModifierData);
        SERIALIZABLE(ConnectionModifierData, "assignFrom", ModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        ConnectionModifierData() = default;
        ConnectionModifierData(Deserializer& deserializer);
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        ElementId m_sourceId;
        FeaturePath m_pathToSourceFeature;
    };
}
