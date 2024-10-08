/**
 * Specialized instance handling for EnumTypes using the ENUM_DEFINE_CPP_ENUM macro.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Instance/instanceTemplates.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

namespace babelwires {

    template<typename T>
    concept HasCppEnum = requires(T) { T::s_hasCppEnum; };

    /// Specialized instance handling for EnumTypes when the type _does_ use the ENUM_DEFINE_CPP_ENUM macro.
    template <typename VALUE_FEATURE, typename ENUM_TYPE>
        requires std::is_base_of_v<EnumType, ENUM_TYPE> && HasCppEnum<ENUM_TYPE>
    class InstanceImpl<VALUE_FEATURE, ENUM_TYPE> : public InstanceCommonBase<VALUE_FEATURE, ENUM_TYPE> {
      public:
        InstanceImpl(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, ENUM_TYPE>(valueFeature) {}

        typename ENUM_TYPE::Value get() const {
            const babelwires::EnumValue& enumValue =
                this->m_valueFeature.getValue()->template is<babelwires::EnumValue>();
            const ENUM_TYPE& enumType = this->m_valueFeature.getType().template is<ENUM_TYPE>();
            return enumType.getValueFromIdentifier(enumValue.get());
        }

        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(typename ENUM_TYPE::Value newValue) {
            const ENUM_TYPE& enumType = this->m_valueFeature.getType().template is<ENUM_TYPE>();
            this->m_valueFeature.setValue(babelwires::EnumValue(enumType.getIdentifierFromValue(newValue)));
        }
    };

    /// Specialized instance handling for EnumTypes when the type _does not_ use the ENUM_DEFINE_CPP_ENUM macro.
    template <typename VALUE_FEATURE, typename ENUM_TYPE>
        requires std::is_base_of_v<EnumType, ENUM_TYPE> && (!HasCppEnum<ENUM_TYPE>)
    class InstanceImpl<VALUE_FEATURE, ENUM_TYPE> : public InstanceCommonBase<VALUE_FEATURE, ENUM_TYPE> {
      public:
        InstanceImpl(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, ENUM_TYPE>(valueFeature) {}

        EnumValue get() const {
            return this->m_valueFeature.getValue()->template is<babelwires::EnumValue>();
        }

        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(EnumValue newValue) {
            const ENUM_TYPE& enumType = this->m_valueFeature.getType().template is<ENUM_TYPE>();
            this->m_valueFeature.setValue(newValue);
        }
    };
} // namespace babelwires
