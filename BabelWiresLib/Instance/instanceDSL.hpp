/**
 * Macros for a DSL for defining instance classes for Record-like types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Instance/instanceUtils.hpp>

#include <optional>

// Macros for a DSL (Domain specific language) for defining instance classes for Record-like types.

/// Start the declaration of an instance.
#define DECLARE_INSTANCE_BEGIN(TYPE)                                                                                   \
    template <typename VALUE_FEATURE> class InstanceImpl : public babelwires::InstanceParent<VALUE_FEATURE, TYPE> {    \
      public:                                                                                                          \
        InstanceImpl(VALUE_FEATURE& valueFeature)                                                                      \
            : babelwires::InstanceParent<VALUE_FEATURE, TYPE>(valueFeature) {}

/// Declare a (non-optional) field.
#define DECLARE_INSTANCE_FIELD(FIELD_NAME, VALUE_TYPE)                                                                 \
    babelwires::ConstInstance<VALUE_TYPE> get##FIELD_NAME() const {                                                    \
        return babelwires::InstanceUtils::getChild(this->m_valueFeature, #FIELD_NAME);                                 \
    }                                                                                                                  \
    template <typename VALUE_FEATURE_M = VALUE_FEATURE>                                                                \
    std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, babelwires::Instance<VALUE_TYPE>> get##FIELD_NAME() {          \
        return babelwires::InstanceUtils::getChild(this->m_valueFeature, #FIELD_NAME);                                 \
    }

/// Declare an optional field.
#define DECLARE_INSTANCE_FIELD_OPTIONAL(FIELD_NAME, VALUE_TYPE)                                                        \
    DECLARE_INSTANCE_FIELD(FIELD_NAME, VALUE_TYPE)                                                                     \
    std::optional<babelwires::ConstInstance<VALUE_TYPE>> tryGet##FIELD_NAME() const {                                  \
        if (const babelwires::ValueFeature* valueFeature =                                                             \
                babelwires::InstanceUtils::tryGetChild(this->m_valueFeature, #FIELD_NAME)) {                           \
            return {*valueFeature};                                                                                    \
        } else {                                                                                                       \
            return {};                                                                                                 \
        }                                                                                                              \
    }                                                                                                                  \
    template <typename VALUE_FEATURE_M = VALUE_FEATURE>                                                                \
    std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, babelwires::Instance<VALUE_TYPE>>                              \
        activateAndGet##FIELD_NAME() {                                                                                 \
        return babelwires::InstanceUtils::activateAndGetChild(this->m_valueFeature, #FIELD_NAME);                      \
    }

/// Conclude the declaration.
#define DECLARE_INSTANCE_END()                                                                                         \
    }                                                                                                                  \
    ;                                                                                                                  \
    using Instance = InstanceImpl<babelwires::ValueFeature>;                                                           \
    using ConstInstance = InstanceImpl<const babelwires::ValueFeature>;
