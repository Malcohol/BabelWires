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
    template <typename VALUE_TREE_NODE, typename VALUE_TYPE>
    class InstanceImpl : public VALUE_TYPE::template InstanceImpl<VALUE_TREE_NODE> {
      public:
        InstanceImpl(VALUE_TREE_NODE& valueFeature)
            : VALUE_TYPE::template InstanceImpl<VALUE_TREE_NODE>(valueFeature) {}
    };

    template <typename VALUE_TYPE> using Instance = InstanceImpl<ValueTreeNode, VALUE_TYPE>;

    template <typename VALUE_TYPE> using ConstInstance = InstanceImpl<const ValueTreeNode, VALUE_TYPE>;

    /// Provides a base of methods which don't depend on the VALUE_TYPE and which can therefore
    /// be used for generic fields which don't have instance specializations.
    template <typename VALUE_TREE_NODE> class InstanceUntypedBase {
      public:
        InstanceUntypedBase(VALUE_TREE_NODE& valueFeature)
            : m_valueTreeNode(valueFeature) {}
        /// Access the const functionality of the ValueTreeNode.
        const VALUE_TREE_NODE* operator->() const { return &m_valueTreeNode; }
        VALUE_TREE_NODE& operator*() const { return m_valueTreeNode; }
        /// Access the non-const functionality of the ValueTreeNode.
        template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>
        std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, VALUE_TREE_NODE*> operator->() {
            return &m_valueTreeNode;
        }

      protected:
        VALUE_TREE_NODE& m_valueTreeNode;
    };

    /// Methods that should be available for every instance.
    /// Inner-class instances and specializations should always inherit from this.
    template <typename VALUE_TREE_NODE, typename VALUE_TYPE>
    class InstanceCommonBase : public InstanceUntypedBase<VALUE_TREE_NODE> {
      public:
        InstanceCommonBase(VALUE_TREE_NODE& valueFeature)
            : InstanceUntypedBase<VALUE_TREE_NODE>(valueFeature) {
            assert(this->m_valueTreeNode.getType()->template tryAs<VALUE_TYPE>());
        }
        /// More specific than this->getType()->
        const VALUE_TYPE& getInstanceType() const { return this->m_valueTreeNode.getType()->template as<VALUE_TYPE>(); }
    };

    /// Can be specialized to make additional methods available for inner-class defined instances of particular types.
    /// For example, RecordWithVariantType classes use this to have common methods (for variant handling) while having
    /// bespoke fields defined by the instanceDSL macros.
    template <typename VALUE_TREE_NODE, typename VALUE_TYPE>
    class InstanceParent : public InstanceCommonBase<VALUE_TREE_NODE, VALUE_TYPE> {
      public:
        InstanceParent(VALUE_TREE_NODE& valueFeature)
            : InstanceCommonBase<VALUE_TREE_NODE, VALUE_TYPE>(valueFeature) {}
    };

} // namespace babelwires
