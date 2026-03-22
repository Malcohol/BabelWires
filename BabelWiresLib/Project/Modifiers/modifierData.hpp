/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <BaseLib/Cloning/cloneable.hpp>
#include <BaseLib/Result/result.hpp>
#include <BaseLib/Serialization/serializable.hpp>
#include <BaseLib/Utilities/downcastable.hpp>

namespace babelwires {

    class Project;
    class Modifier;
    class FilePath;

    /// ModifierData carry the data sufficient to reconstruct a Modifier.
    struct BABELWIRESLIB_API ModifierData : Cloneable, Serializable, ProjectVisitable {
        CLONEABLE_ABSTRACT(ModifierData);
        SERIALIZABLE_ABSTRACT(ModifierData, void);
        DOWNCASTABLE_BASE(ModifierData);

        /// Identifies the ValueTreeNode being modified.
        Path m_targetPath;

        /// Find the target ValueTreeNode within the container.
        ResultT<ValueTreeNode&> getTarget(ValueTreeNode* container) const;

        /// A factory method constructing the appropriate modifier.
        virtual std::unique_ptr<Modifier> createModifier() const = 0;

        /// Call the visitor on all fields in the modifier.
        /// This base implementation visits the fields in m_targetPath;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        /// This does nothing.
        /// (There is currently no scenario where a modifier references a filepath, but
        /// this is just here for future proofing.)
        void visitFilePaths(FilePathVisitor& visitor) override;
    };

    /// Base class for ModifierData which construct LocalModifiers.
    struct BABELWIRESLIB_API LocalModifierData : ModifierData {
        DOWNCASTABLE(LocalModifierData, ModifierData);
        CLONEABLE_ABSTRACT(LocalModifierData);
        SERIALIZABLE_ABSTRACT(LocalModifierData, ModifierData);

        /// Perform the modification on the target ValueTreeNode.
        virtual Result apply(ValueTreeNode* target) const = 0;

        virtual std::unique_ptr<Modifier> createModifier() const;
    };
} // namespace babelwires
