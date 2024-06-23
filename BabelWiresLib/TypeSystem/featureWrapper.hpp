/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#define FEATURE_WRAPPER_BEGIN(TYPE)                                                                                    \
    template <typename VALUE_FEATURE> class FeatureWrapper {                                                           \
        VALUE_FEATURE* m_valueFeature;                                                                                 \
                                                                                                                       \
      public:                                                                                                          \
        FeatureWrapper(VALUE_FEATURE* valueFeature)                                                                    \
            : m_valueFeature(valueFeature) {                                                                           \
            assert(valueFeature->getType().template as<TYPE>());                                                       \
        }                                                                                                              \
        operator bool() const {                                                                                        \
            return m_valueFeature;                                                                                     \
        }

#define FEATURE_WRAPPER_END()                                                                                          \
    }                                                                                                                  \
    ;

namespace babelwires {
    /// The default approach to finding feature wrappers is this template.
    /// It "despatches to" (i.e. inherits from) a corresponding inner-class
    /// in the type. However, some types (e.g. built-ins) may prefer to
    /// specialize the template instead.
    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class FeatureWrapper : public VALUE_TYPE::FeatureWrapper<VALUE_FEATURE> {
      public:
        FeatureWrapper(VALUE_FEATURE* valueFeature)
            : VALUE_TYPE::FeatureWrapper<VALUE_FEATURE>(valueFeature) {}
    };
} // namespace babelwires
