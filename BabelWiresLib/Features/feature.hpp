/**
 * A Feature is a self-describing data-structure which stores the data in the model.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Utilities/enumFlags.hpp>
#include <Common/types.hpp>

#include <BabelWiresLib/Features/Path/pathStep.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace babelwires {

    class CompoundFeature;

    /// A feature is a self-describing data-structure which stores the data in the model.
    /// Features are structured in a tree, which also defines ownership.
    class Feature {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Feature);

        Feature() = default;
        virtual ~Feature();

        void setOwner(CompoundFeature* owner);
        const CompoundFeature* getOwner() const;

        /// Set to the default value.
        void setToDefault();

        /// Set to this to default but, in the case of compound features, leave remaining subfeatures untouched.
        void setToDefaultNonRecursive();

        /// Describes the way a feature may have changed.
        enum class Changes : unsigned int {
            NothingChanged = 0b0000,
            ValueChanged = 0b0001,
            StructureChanged = 0b0010,
            SomethingChanged = 0b0011
        };

        /// Has this feature or its children changed in any of the ways described by changes?
        bool isChanged(Changes changes) const;

        /// Clear the change flags of this feature and all its children.
        void clearChanges();

        /// Get a hash of the feature contents _of a feature of this type_.
        /// This is not required to distinguish the contents of features of different types.
        std::size_t getHash() const;

        /// Flags defining aspects of how a feature appears in a feature element.
        enum class Style : unsigned int {
            // Applies only to compound features.
            /// By default, compounds are collapsed and can be expanded. If false, they will be in a permanently expanded state.
            isCollapsable = 0b0001,
            /// By default, the children of compounds are indented. If true, this will present the children at the top level.
            isInlined = 0b0010,
        };

        /// How should this feature appear in a feature element?
        /// The system assumes that styles do not change over the lifetime of a feature.
        virtual Style getStyle() const;

      protected:
        /// Set the isChanged flag and that of all parents.
        void setChanged(Changes changes);

        /// Protected implementation of setToDefault.
        virtual void doSetToDefault() = 0;

        /// Protected implementation of setToDefaultNonRecursive.
        virtual void doSetToDefaultNonRecursive() = 0;

        /// Protected implementation of getHash.
        virtual std::size_t doGetHash() const = 0;

        /// Protected implementation which sets this class' changes to NothingChanged.
        virtual void doClearChanges();

        CompoundFeature* getOwnerNonConst();
      private:
        // For now.
        Feature(const Feature&) = delete;
        Feature& operator=(const Feature&) = delete;

      private:
        CompoundFeature* m_owner = nullptr;
        Changes m_changes = Changes::SomethingChanged;
    };

    DEFINE_ENUM_FLAG_OPERATORS(Feature::Changes);
    DEFINE_ENUM_FLAG_OPERATORS(Feature::Style);

} // namespace babelwires
