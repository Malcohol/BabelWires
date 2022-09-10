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
            /// Applies only to compound features.
            isCollapsable = 0b0001,
        };

        /// How should this feature appear in a feature element?
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

      private:
        // For now.
        Feature(const Feature&) = delete;
        Feature& operator=(const Feature&) = delete;

      private:
        CompoundFeature* m_owner = nullptr;
        Changes m_changes = Changes::SomethingChanged;
    };

    /// A value feature is one which can be set.
    class ValueFeature : public Feature {
      public:
        /// A short string which determines which values can be passed in to assign.
        /// TODO: This design is imposed by the current UI, but is inflexible because it doesn't
        /// support a good notion of subtyping.
        std::string getValueType() const;

        /// Could the value in other ever be assigned to this (irrespective of its current value).
        /// This checks that the value types are equal.
        bool isCompatible(const ValueFeature& other);

        /// Set this to hold the same value as other.
        /// This will throw a ModelException if the assignment failed.
        void assign(const ValueFeature& other);

      protected:
        /// Calls doSetToDefault.
        virtual void doSetToDefaultNonRecursive() override;

        /// Return a string of length <= 4 characters.
        virtual std::string doGetValueType() const = 0;

        /// Implementations may assume that other is compatible.
        virtual void doAssign(const ValueFeature& other) = 0;
    };

    /// A feature which contains other features.
    /// See function subfeatures in modelUtilities for support for iterators and ranged for loops.
    class CompoundFeature : public Feature {
      public:
        virtual int getNumFeatures() const = 0;
        Feature* getFeature(int i);
        const Feature* getFeature(int i) const;

        virtual PathStep getStepToChild(const Feature* child) const = 0;

        /// Should return nullptr if the step does not lead to a child.
        virtual Feature* tryGetChildFromStep(const PathStep& step) = 0;

        /// Should return nullptr if the step does not lead to a child.
        virtual const Feature* tryGetChildFromStep(const PathStep& step) const = 0;

        /// Throws a ModelException if the step does not lead to a child.
        Feature& getChildFromStep(const PathStep& step);

        /// Throws a ModelException if the step does not lead to a child.
        const Feature& getChildFromStep(const PathStep& step) const;

      protected:
        /// Clears the changes of this class and all children.
        virtual void doClearChanges() override;
        virtual Feature* doGetFeature(int i) = 0;
        virtual const Feature* doGetFeature(int i) const = 0;

      protected:
        /// Call setToDefault on each subfeature.
        void setSubfeaturesToDefault();
    };

    DEFINE_ENUM_FLAG_OPERATORS(Feature::Changes);
    DEFINE_ENUM_FLAG_OPERATORS(Feature::Style);

} // namespace babelwires
