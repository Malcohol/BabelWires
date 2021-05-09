/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/Modifiers/modifierData.hpp"

namespace babelwires {
    /// Data used to assign a ValueFeature within a container to a value from another file.
    struct AssignFromFeatureData : ModifierData {
        /// Find the source feature in the project, or throw.
        const Feature* getSourceFeature(const Project& project) const;

        /// Apply the source value to the target feature.
        void apply(const Feature* sourceFeature, Feature* targetFeature, bool applyEvenIfSourceUnchanged) const;

        virtual std::unique_ptr<Modifier> createModifier() const;

        CLONEABLE(AssignFromFeatureData);
        SERIALIZABLE(AssignFromFeatureData, "assignFrom", ModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        ElementId m_sourceId;
        FeaturePath m_pathToSourceFeature;
    };
}
