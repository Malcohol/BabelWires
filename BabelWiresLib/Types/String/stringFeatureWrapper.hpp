/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

namespace babelwires {
    template <typename VALUE_FEATURE, typename T>
        requires std::is_base_of_v<StringType, T>
    class FeatureWrapper<VALUE_FEATURE, T> {
        VALUE_FEATURE* m_valueFeature;

      public:
        FeatureWrapper(VALUE_FEATURE* valueFeature)
            : m_valueFeature(valueFeature) {
            assert(!valueFeature || valueFeature->getType().template as<StringType>());
        }
        operator bool() const { return m_valueFeature; }

        std::string get() const {
            assert(m_valueFeature);
            const StringValue& stringValue = m_valueFeature->getValue()->template is<StringValue>();
            return stringValue.get();
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(std::string newValue) {
            assert(m_valueFeature);
            m_valueFeature->setValue(StringValue(std::move(newValue)));
        }
    };

} // namespace babelwires
