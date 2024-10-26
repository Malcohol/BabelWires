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
#include <Common/multiKeyMap.hpp>

#include <BabelWiresLib/Features/Path/pathStep.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace babelwires {
    class Type;
    class SimpleValueFeature;
    class ChildValueFeature;

    /// A feature is a self-describing data-structure which stores the data in the model.
    /// Features are structured in a tree, which also defines ownership.
    class Feature {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Feature);

        Feature(TypeRef typeRef);
        virtual ~Feature();

        void setOwner(Feature* owner);
        const Feature* getOwner() const;
        Feature* getOwnerNonConst();

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

      public:
        virtual int getNumFeatures() const;

        Feature* getFeature(int i);
        const Feature* getFeature(int i) const;

        virtual PathStep getStepToChild(const Feature* child) const;

        /// Should return nullptr if the step does not lead to a child.
        Feature* tryGetChildFromStep(const PathStep& step);

        /// Should return nullptr if the step does not lead to a child.
        const Feature* tryGetChildFromStep(const PathStep& step) const;

        /// Throws a ModelException if the step does not lead to a child.
        Feature& getChildFromStep(const PathStep& step);

        /// Throws a ModelException if the step does not lead to a child.
        const Feature& getChildFromStep(const PathStep& step) const;

        /// Returns -1 if not found.
        /// Sets the descriminator of identifier on a match.
        virtual int getChildIndexFromStep(const PathStep& step) const;

      public:
        /// Get the TypeRef which describes the type of the value.
        const TypeRef& getTypeRef() const;

        /// Get the value currently held by this feature.
        const ValueHolder& getValue() const;

        /// Set this feature to hold a new value.
        void setValue(const ValueHolder& newValue);

        /// Value features always exist within a hierarchy where the root carries a reference to the TypeSystem.
        const TypeSystem& getTypeSystem() const;

        /// This is a convenience method which resolves the typeRef in the context of the TypeSystem
        /// carried by the root.
        const Type& getType() const;

        /// This is a convenience method which calls getType().getKind().
        /// The need for connectable features to provide a string description is not fundamental to the data model.
        /// It is imposed by the current UI.
        /// Returning the empty string tells the project that values of this kind cannot be wired together.
        std::string getKind() const;

        /// Set this to hold the same value as other.
        /// This will throw a ModelException if the assignment failed.
        void assign(const Feature& other);

        /// If the value is compound, synchronize the m_children data structure with the current children of the value.
        void synchronizeSubfeatures();

        /// Set change flags in this feature and its subfeatures by comparing the current value with that of other.
        void reconcileChanges(const ValueHolder& other);

      protected:
        /// Set the isChanged flag and that of all parents.
        void setChanged(Changes changes);

        /// Protected implementation of setToDefault.
        virtual void doSetToDefault() = 0;

        /// Protected implementation of setToDefaultNonRecursive.
        virtual void doSetToDefaultNonRecursive();

        /// Protected implementation of getHash.
        virtual std::size_t doGetHash() const;

        /// Clears the changes of this class and all children.
        virtual void doClearChanges();

        /// Call setToDefault on each subfeature.
        void setSubfeaturesToDefault();

      protected:
        virtual Feature* doGetFeature(int i);
        virtual const Feature* doGetFeature(int i) const;

      protected:
        virtual const ValueHolder& doGetValue() const = 0;
        virtual void doSetValue(const ValueHolder& newValue) = 0;

      private:
        // For now.
        Feature(const Feature&) = delete;
        Feature& operator=(const Feature&) = delete;

      private:
        Feature* m_owner = nullptr;
        Changes m_changes = Changes::SomethingChanged;

        TypeRef m_typeRef;
        using ChildMap = MultiKeyMap<PathStep, unsigned int, std::unique_ptr<ChildValueFeature>>;
        ChildMap m_children;
    };

    DEFINE_ENUM_FLAG_OPERATORS(Feature::Changes);

} // namespace babelwires
