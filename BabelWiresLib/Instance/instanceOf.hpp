/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#define DECLARE_INSTANCE_BEGIN(TYPE)                                                                                   \
    template <typename VALUE_FEATURE> class Instance : public babelwires::InstanceParent<VALUE_FEATURE, TYPE> {        \
      public:                                                                                                          \
        Instance(VALUE_FEATURE* valueFeature)                                                                          \
            : babelwires::InstanceParent<VALUE_FEATURE, TYPE>(valueFeature) {}

#define DECLARE_INSTANCE_END()                                                                                         \
    }                                                                                                                  \
    ;

namespace babelwires {
    /// The default approach to finding feature wrappers is this template.
    /// It "despatches to" (i.e. inherits from) a corresponding inner-class
    /// in the type. However, some types (e.g. built-ins) may prefer to
    /// specialize the template instead.
    template <typename VALUE_FEATURE, typename VALUE_TYPE> class Instance : public VALUE_TYPE::Instance<VALUE_FEATURE> {
      public:
        Instance(VALUE_FEATURE* valueFeature)
            : VALUE_TYPE::Instance<VALUE_FEATURE>(valueFeature) {}
    };

    /// Methods that should be available for every instance.
    template <typename VALUE_FEATURE, typename VALUE_TYPE> class InstanceCommonBase {
      public:
        InstanceCommonBase(VALUE_FEATURE* valueFeature)
            : m_valueFeature(valueFeature) {
            assert(!valueFeature || valueFeature->getType().template as<VALUE_TYPE>());
        }
        const VALUE_TYPE& getInstanceType() const { return m_valueFeature->getType().template is<VALUE_TYPE>(); }
        const babelwires::ValueHolder& getInstanceValue() const { return m_valueFeature->getValue(); }

      protected:
        VALUE_FEATURE* m_valueFeature;
    };

    /// Can be specialized to make additional methods available for instances of particular types.
    template <typename VALUE_FEATURE, typename VALUE_TYPE> class InstanceParent : public InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE> {
      public:
        InstanceParent(VALUE_FEATURE* valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE>(valueFeature) {}
    };

} // namespace babelwires
