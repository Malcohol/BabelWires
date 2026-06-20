/**
 * Specialized instance handling for TextType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

namespace babelwires {
    /// Specialized instance handling for TextType.
    template <typename VALUE_TREE_NODE, typename STRING_TYPE>
        requires std::is_base_of_v<TextType, STRING_TYPE>
    class InstanceImpl<VALUE_TREE_NODE, STRING_TYPE> : public InstanceCommonBase<VALUE_TREE_NODE, STRING_TYPE> {
      public:
        InstanceImpl(VALUE_TREE_NODE& valueFeature)
            : InstanceCommonBase<VALUE_TREE_NODE, STRING_TYPE>(valueFeature) {}

        Text get() const {
            const TextValue& textValue = this->m_valueTreeNode.getValue()->template as<TextValue>();
            return textValue.get();
        }
        template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>
        std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, void> set(Text newValue) {
            this->m_valueTreeNode.assertSetValue(TextValue(std::move(newValue)));
        }
    };

} // namespace babelwires
