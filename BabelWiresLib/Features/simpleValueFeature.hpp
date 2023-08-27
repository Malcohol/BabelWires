/**
 * A SimpleValueFeature is a ValueFeature which owns its value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Type;
    class Value;

    /// A SimpleValueFeature is a feature which owns its value.
    class SimpleValueFeature : public ValueFeature {
      public:
        /// Construct a ValueFeature which carries values of the given type.
        SimpleValueFeature(TypeRef typeRef);

        /// Create or access a stored non-const copy of the contained value
        ValueHolder& getNonConstValueCopy();

        /// Apply the copy to the contained value.
        void applyValueCopy();

      protected:
        const ValueHolder& doGetValue() const override;

      private:
        ValueHolder m_value;
        ValueHolder m_valueCopy;
    };
} // namespace babelwires
