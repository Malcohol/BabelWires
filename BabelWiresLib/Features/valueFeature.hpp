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
        virtual const TypeRef& getTypeRef() const = 0;
        virtual const ValueHolder& getValue() const = 0;
        virtual void setValue(const ValueHolder& newValue) = 0;

        /// This is a convenience method which resolves the typeRef in the context of the TypeSystem
        /// carried by the rootFeature.
        const Type& getType() const;

        /// This is a convenience method which calls getType().getKind().
        /// TODO: This design is imposed by the current UI, but is inflexible because it doesn't
        /// support a good notion of subtyping.
        std::string getKind() const;

        /// Set this to hold the same value as other.
        /// This will throw a ModelException if the assignment failed.
        void assign(const ValueFeature& other);

      protected:
        /// Calls doSetToDefault.
        virtual void doSetToDefaultNonRecursive() override;
    };
}