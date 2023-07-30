/**
 * A ChildValueFeature is a ValueFeature whose value is owned by an ancestor feature.
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

    /// A ChildValueFeature is a ValueFeature whose value is owned by an ancestor feature.
    class ChildValueFeature : public ValueFeature {
      public:
        /// Construct a ValueFeature which carries values of the given type.
        ChildValueFeature(TypeRef typeRef, const ValueHolder& valueHolder);

      protected:
        const ValueHolder& doGetValue() const override;

      private:
        const ValueHolder& m_value;
    };
} // namespace babelwires
