/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

namespace babelwires {
    template <typename VALUE_FEATURE, typename T>
        requires std::is_base_of_v<IntType, T>
    class Instance<VALUE_FEATURE, T> {
        VALUE_FEATURE* m_valueFeature;

      public:
        Instance(VALUE_FEATURE* valueFeature)
            : m_valueFeature(valueFeature) {
            assert(!valueFeature || valueFeature->getType().template as<IntType>());
        }

        typename IntValue::NativeType get() const {
            assert(m_valueFeature);
            const IntValue& intValue = m_valueFeature->getValue()->template is<IntValue>();
            return intValue.get();
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(typename IntValue::NativeType newValue) {
            assert(m_valueFeature);
            m_valueFeature->setValue(IntValue(newValue));
        }
    };

} // namespace babelwires
