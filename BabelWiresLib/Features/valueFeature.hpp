/**
 * A ValueFeature is a feature which stores a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/feature.hpp>

namespace babelwires {
    /// A ValueFeature is a feature which stores a value.
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
}