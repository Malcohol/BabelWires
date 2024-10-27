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
    template <typename VALUE_FEATURE, typename STRING_TYPE>
        requires std::is_base_of_v<StringType, STRING_TYPE>
    class InstanceImpl<VALUE_FEATURE, STRING_TYPE> : public InstanceCommonBase<VALUE_FEATURE, STRING_TYPE> {
      public:
        InstanceImpl(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, STRING_TYPE>(valueFeature) {}

        std::string get() const {
            const StringValue& stringValue = this->m_valueTreeNode.getValue()->template is<StringValue>();
            return stringValue.get();
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(std::string newValue) {
            this->m_valueTreeNode.setValue(StringValue(std::move(newValue)));
        }
    };

} // namespace babelwires
