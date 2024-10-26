/**
 * The core templates used by the instance system.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    /// The default approach to finding the appropriate instance class is this template.
    /// For record-style types it "dispatches to" (i.e. inherits from) a corresponding inner-class
    /// in the type's class. However, some types (e.g. built-ins) specialize the template instead.
    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class InstanceImpl : public VALUE_TYPE::template InstanceImpl<VALUE_FEATURE> {
      public:
        InstanceImpl(VALUE_FEATURE& valueFeature)
            : VALUE_TYPE::template InstanceImpl<VALUE_FEATURE>(valueFeature) {}
    };

    template <typename VALUE_TYPE> using Instance = InstanceImpl<ValueTreeNode, VALUE_TYPE>;

    template <typename VALUE_TYPE> using ConstInstance = InstanceImpl<const ValueTreeNode, VALUE_TYPE>;

    /// Provides a base of methods which don't depend on the VALUE_TYPE and which can therefore
    /// be used for generic fields which don't have instance specializations.
    template <typename VALUE_FEATURE> class InstanceUntypedBase {
      public:
        InstanceUntypedBase(VALUE_FEATURE& valueFeature)
            : m_valueFeature(valueFeature) {}
        /// Access the functionality of the Feature.
        const VALUE_FEATURE* operator->() const { return &m_valueFeature; }
        VALUE_FEATURE& operator*() const { return m_valueFeature; }
        /// Access the functionality of the Feature.
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, VALUE_FEATURE*> operator->() {
            return &m_valueFeature;
        }

      protected:
        VALUE_FEATURE& m_valueFeature;
    };

    /// Methods that should be available for every instance.
    /// Inner-class instances and specializations should always inherit from this.
    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class InstanceCommonBase : public InstanceUntypedBase<VALUE_FEATURE> {
      public:
        InstanceCommonBase(VALUE_FEATURE& valueFeature)
            : InstanceUntypedBase<VALUE_FEATURE>(valueFeature) {
            assert(this->m_valueFeature.getType().template as<VALUE_TYPE>());
        }
        /// More specific than this->getType().
        const VALUE_TYPE& getInstanceType() const { return this->m_valueFeature.getType().template is<VALUE_TYPE>(); }
    };

    /// Can be specialized to make additional methods available for inner-class defined instances of particular types.
    /// For example, RecordWithVariantType classes use this to have common methods (for variant handling) while having
    /// bespoke fields defined by the instanceDSL macros.
    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class InstanceParent : public InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE> {
      public:
        InstanceParent(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE>(valueFeature) {}
    };

} // namespace babelwires
