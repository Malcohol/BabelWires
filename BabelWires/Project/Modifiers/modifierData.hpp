/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Features/Path/featurePath.hpp"
#include "BabelWires/Project/projectIds.hpp"
#include "Common/Cloning/cloneable.hpp"
#include "Common/Math/rational.hpp"
#include "Common/Serialization/serializable.hpp"
namespace babelwires {

    class Project;
    class Modifier;

    /// ModifierData carry the data sufficient to reconstruct a Modifier.
    struct ModifierData : Cloneable, Serializable {
        CLONEABLE_ABSTRACT(ModifierData);
        SERIALIZABLE_ABSTRACT(ModifierData, "ModifierData", void);

        /// Identifies the feature being modified.
        FeaturePath m_pathToFeature;

        /// Find the target feature within the container, or throw.
        Feature* getTargetFeature(Feature* container) const;

        /// A factory method constructing the appropriate modifier.
        virtual std::unique_ptr<Modifier> createModifier() const = 0;
    };

    /// Base class for ModifierData which construct LocalModifiers.
    struct LocalModifierData : ModifierData {
        CLONEABLE_ABSTRACT(LocalModifierData);
        SERIALIZABLE_ABSTRACT(LocalModifierData, "LocalModifierData", ModifierData);

        /// Perform the modification on the target feature, or throw.
        virtual void apply(Feature* targetFeature) const = 0;

        virtual std::unique_ptr<Modifier> createModifier() const;
    };

    /// Data used to assign an IntFeature within a container to a certain value.
    struct IntValueAssignmentData : LocalModifierData {
        virtual void apply(Feature* targetFeature) const override;
        CLONEABLE(IntValueAssignmentData);
        SERIALIZABLE(IntValueAssignmentData, "assignInt", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        int m_value = 0;
    };

    /// Data used to assign an IntFeature within a container to a certain value.
    struct RationalValueAssignmentData : LocalModifierData {
        virtual void apply(Feature* targetFeature) const override;
        CLONEABLE(RationalValueAssignmentData);
        SERIALIZABLE(RationalValueAssignmentData, "assignRational", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        Rational m_value = 0;
    };

    /// Data used to assign a StringFeature within a container to a certain value.
    struct StringValueAssignmentData : LocalModifierData {
        virtual void apply(Feature* targetFeature) const override;
        CLONEABLE(StringValueAssignmentData);
        SERIALIZABLE(StringValueAssignmentData, "assignString", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        std::string m_value;
    };

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

} // namespace babelwires
