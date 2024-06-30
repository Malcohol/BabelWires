/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#define FEATURE_WRAPPER_BEGIN_WITH_PARENT(TYPE, PARENT)                                                                \
    template <typename VALUE_FEATURE> class Instance : public PARENT {                                           \
      public:                                                                                                          \
        Instance(VALUE_FEATURE* valueFeature)                                                                    \
            : PARENT(valueFeature) {}

#define MACRO_COMMA ,

#define FEATURE_WRAPPER_BEGIN(TYPE)                                                                                    \
    FEATURE_WRAPPER_BEGIN_WITH_PARENT(TYPE, babelwires::FeatureWrapperCommonBase<VALUE_FEATURE MACRO_COMMA TYPE>)

#define FEATURE_WRAPPER_END()                                                                                          \
    }                                                                                                                  \
    ;

namespace babelwires {
    /// The default approach to finding feature wrappers is this template.
    /// It "despatches to" (i.e. inherits from) a corresponding inner-class
    /// in the type. However, some types (e.g. built-ins) may prefer to
    /// specialize the template instead.
    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class Instance : public VALUE_TYPE::Instance<VALUE_FEATURE> {
      public:
        Instance(VALUE_FEATURE* valueFeature)
            : VALUE_TYPE::Instance<VALUE_FEATURE>(valueFeature) {}
    };

    template <typename VALUE_FEATURE, typename VALUE_TYPE> class FeatureWrapperCommonBase {
      public:
        FeatureWrapperCommonBase(VALUE_FEATURE* valueFeature)
            : m_valueFeature(valueFeature) {
            assert(!valueFeature || valueFeature->getType().template as<VALUE_TYPE>());
        }
        const VALUE_TYPE& getInstanceType() const { return m_valueFeature->getType().template is<VALUE_TYPE>(); }
        const babelwires::ValueHolder& getInstanceValue() const { return m_valueFeature->getValue(); }

      protected:
        VALUE_FEATURE* m_valueFeature;
    };

} // namespace babelwires
