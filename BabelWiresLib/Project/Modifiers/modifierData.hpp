/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Serialization/serializable.hpp>

namespace babelwires {

    class Project;
    class Modifier;
    class FilePath;

    /// ModifierData carry the data sufficient to reconstruct a Modifier.
    struct ModifierData : Cloneable, Serializable, ProjectVisitable {
        CLONEABLE_ABSTRACT(ModifierData);
        SERIALIZABLE_ABSTRACT(ModifierData, void);
        DOWNCASTABLE_TYPE_HIERARCHY(ModifierData);

        /// Identifies the feature being modified.
        Path m_pathToFeature;

        /// Find the target feature within the container, or throw.
        Feature* getTargetFeature(Feature* container) const;

        /// A factory method constructing the appropriate modifier.
        virtual std::unique_ptr<Modifier> createModifier() const = 0;

        /// Call the visitor on all fields in the modifier.
        /// This base implementation visits the fields in m_pathToFeature;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        /// This does nothing.
        /// (There is currently no scenario where a modifier references a filepath, but
        /// this is just here for future proofing.)
        void visitFilePaths(FilePathVisitor& visitor) override;
    };

    /// Base class for ModifierData which construct LocalModifiers.
    struct LocalModifierData : ModifierData {
        CLONEABLE_ABSTRACT(LocalModifierData);
        SERIALIZABLE_ABSTRACT(LocalModifierData, ModifierData);

        /// Perform the modification on the target feature, or throw.
        virtual void apply(Feature* targetFeature) const = 0;

        virtual std::unique_ptr<Modifier> createModifier() const;
    };
} // namespace babelwires
