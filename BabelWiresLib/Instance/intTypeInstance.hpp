/**
 * Specialized instance handling for IntType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    /// Specialized instance handling for IntType.
    template <typename VALUE_FEATURE, typename INT_TYPE>
        requires std::is_base_of_v<IntType, INT_TYPE>
    class InstanceImpl<VALUE_FEATURE, INT_TYPE> : public InstanceCommonBase<VALUE_FEATURE, INT_TYPE> {
      public:
        InstanceImpl(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, INT_TYPE>(valueFeature) {}

        typename IntValue::NativeType get() const {
            const IntValue& intValue = this->m_valueFeature.getValue()->template is<IntValue>();
            return intValue.get();
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(typename IntValue::NativeType newValue) {
            this->m_valueFeature.setValue(IntValue(newValue));
        }
    };

} // namespace babelwires
