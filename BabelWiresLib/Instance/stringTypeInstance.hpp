/**
 * Specialized instance handling for StringType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

namespace babelwires {
    /// Specialized instance handling for StringType.
    template <typename VALUE_TREE_NODE, typename STRING_TYPE>
        requires std::is_base_of_v<StringType, STRING_TYPE>
    class InstanceImpl<VALUE_TREE_NODE, STRING_TYPE> : public InstanceCommonBase<VALUE_TREE_NODE, STRING_TYPE> {
      public:
        InstanceImpl(VALUE_TREE_NODE& valueFeature)
            : InstanceCommonBase<VALUE_TREE_NODE, STRING_TYPE>(valueFeature) {}

        std::string get() const {
            const StringValue& stringValue = this->m_valueTreeNode.getValue()->template is<StringValue>();
            return stringValue.get();
        }
        template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>
        std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, void> set(std::string newValue) {
            this->m_valueTreeNode.setValue(StringValue(std::move(newValue)));
        }
    };

} // namespace babelwires
