/**
 * Specialized instance handling for RationalType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>

namespace babelwires {
    /// Specialized instance handling for RationalType.
    template <typename VALUE_TREE_NODE, typename RATIONAL_TYPE>
        requires std::is_base_of_v<RationalType, RATIONAL_TYPE>
    class InstanceImpl<VALUE_TREE_NODE, RATIONAL_TYPE> : public InstanceCommonBase<VALUE_TREE_NODE, RATIONAL_TYPE> {
      public:
        InstanceImpl(VALUE_TREE_NODE& valueFeature)
            : InstanceCommonBase<VALUE_TREE_NODE, RATIONAL_TYPE>(valueFeature) {}

        Rational get() const {
            const RationalValue& value = this->m_valueTreeNode.getValue()->template is<RationalValue>();
            return value.get();
        }
        template <typename VALUE_FEATURE_M = VALUE_TREE_NODE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(Rational newValue) {
            this->m_valueTreeNode.setValue(RationalValue(newValue));
        }
    };

} // namespace babelwires
