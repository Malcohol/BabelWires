/**
 * Specialized instance handling for FixedType.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Types/Fixed/fixedType.hpp>
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

namespace babelwires {
    /// Specialized instance handling for FixedType.
    template <typename VALUE_TREE_NODE, typename FIXED_TYPE>
        requires std::is_base_of_v<FixedType, FIXED_TYPE>
    class InstanceImpl<VALUE_TREE_NODE, FIXED_TYPE> : public InstanceCommonBase<VALUE_TREE_NODE, FIXED_TYPE> {
      public:
        InstanceImpl(VALUE_TREE_NODE& valueFeature)
            : InstanceCommonBase<VALUE_TREE_NODE, FIXED_TYPE>(valueFeature) {}

        Fixed get() const {
            const FixedValue& fixedValue = this->m_valueTreeNode.getValue()->template as<FixedValue>();
            return fixedValue.get();
        }
        template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>
        std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, void> set(Fixed newValue) {
            this->m_valueTreeNode.assertSetValue(FixedValue(newValue));
        }
    };

} // namespace babelwires
