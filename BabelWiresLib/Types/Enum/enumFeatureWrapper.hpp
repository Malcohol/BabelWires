/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

#define FEATURE_WRAPPER_CPP_ENUM(ENUM_TYPE)                                                                            \
    typename ENUM_TYPE::Value get() const {                                                                            \
        const babelwires::EnumValue& enumValue = m_valueFeature->getValue()->template is<babelwires::EnumValue>();     \
        const ENUM_TYPE& enumType = m_valueFeature->getType().template is<ENUM_TYPE>();                                \
        return enumType.getValueFromIdentifier(enumValue.get());                                                       \
    }                                                                                                                  \
    template <typename VALUE_FEATURE_M = VALUE_FEATURE>                                                                \
    std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> set(typename ENUM_TYPE::Value newValue) {                \
        const ENUM_TYPE& enumType = m_valueFeature->getType().template is<ENUM_TYPE>();                                \
        m_valueFeature->setValue(babelwires::EnumValue(enumType.getIdentifierFromValue(newValue)));                    \
    }
