/**
 * A ValueTreeChild is a ValueTreeNode whose value is owned by an ancestor feature.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Type;
    class Value;

    /// A ValueTreeChild is a ValueTreeNode whose value is owned by an ancestor feature.
    class ValueTreeChild : public ValueTreeNode {
      public:
        /// Construct a ValueTreeNode which carries values of the given type.
        ValueTreeChild(TypeExp typeExp, const ValueHolder& valueHolder, ValueTreeNode* owner);

      protected:
        void doSetValue(const ValueHolder& newValue) override;
        void doSetToDefault() override;
    };
} // namespace babelwires
