/**
 * The core templates used by the instance system.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>

namespace babelwires {
    /// The default approach to finding the appropriate instance class is this template.
    /// For record-style types it "despatches to" (i.e. inherits from) a corresponding inner-class
    /// in the type's class. However, some types (e.g. built-ins) specialize the template instead.
    template <typename VALUE_FEATURE, typename VALUE_TYPE> class Instance : public VALUE_TYPE::Instance<VALUE_FEATURE> {
      public:
        Instance(VALUE_FEATURE& valueFeature)
            : VALUE_TYPE::Instance<VALUE_FEATURE>(valueFeature) {}
    };

    /// Methods that should be available for every instance.
    /// Inner-class instances and specializations should always inherit from this.
    template <typename VALUE_FEATURE, typename VALUE_TYPE> class InstanceCommonBase {
      public:
        InstanceCommonBase(VALUE_FEATURE& valueFeature)
            : m_valueFeature(valueFeature) {
            assert(valueFeature.getType().template as<VALUE_TYPE>());
        }
        const VALUE_TYPE& getInstanceType() const { return m_valueFeature.getType().template is<VALUE_TYPE>(); }
        const babelwires::ValueHolder& getInstanceValue() const { return m_valueFeature.getValue(); }

      protected:
        VALUE_FEATURE& m_valueFeature;
    };

    /// Can be specialized to make additional methods available for inner-class defined instances of particular types.
    /// For example, RecordWithVariantType classes use this to have common methods (for variant handling) while having bespoke
    /// fields defined by the instanceDSL macros.
    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class InstanceParent : public InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE> {
      public:
        InstanceParent(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE>(valueFeature) {}
    };

}
