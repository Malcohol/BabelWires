/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/feature.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    /// A ValueFeature is a feature which provides access to a value.
    class ValueFeature : public Feature {
      public:
        /// Get the TypeRef which describes the type of the value.
        virtual const TypeRef& getTypeRef() const = 0;

        /// Get the value currently held by this feature.
        virtual const ValueHolder& getValue() const = 0;

        /// Set this feature to hold a new value.
        virtual void setValue(const ValueHolder& newValue) = 0;

        /// This is a convenience method which resolves the typeRef in the context of the TypeSystem
        /// carried by the rootFeature.
        const Type& getType() const;

        /// This is a convenience method which calls getType().getKind().
        /// The need for connectable features to provide a string description is not fundamental to the data model.
        /// It is imposed by the current UI.
        std::string getKind() const;

        /// Set this to hold the same value as other.
        /// This will throw a ModelException if the assignment failed.
        void assign(const ValueFeature& other);

      protected:
        /// Calls doSetToDefault.
        virtual void doSetToDefaultNonRecursive() override;
    };
}