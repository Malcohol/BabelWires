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
    template <typename VALUE_TREE_NODE>                                                                                \
    class InstanceImpl : public babelwires::InstanceParent<VALUE_TREE_NODE, TYPE> {                                    \
      public:                                                                                                          \
        InstanceImpl(VALUE_TREE_NODE& valueFeature)                                                                    \
            : babelwires::InstanceParent<VALUE_TREE_NODE, TYPE>(valueFeature) {}

/// Declare a (non-optional) field.
#define DECLARE_INSTANCE_FIELD(FIELD_NAME, VALUE_TYPE)                                                                 \
    babelwires::ConstInstance<VALUE_TYPE> get##FIELD_NAME() const {                                                    \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }                                                                                                                  \
    template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>                                                            \
    std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, babelwires::Instance<VALUE_TYPE>> get##FIELD_NAME() {        \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }

/// Declare an optional field.
#define DECLARE_INSTANCE_FIELD_OPTIONAL(FIELD_NAME, VALUE_TYPE)                                                        \
    DECLARE_INSTANCE_FIELD(FIELD_NAME, VALUE_TYPE)                                                                     \
    std::optional<babelwires::ConstInstance<VALUE_TYPE>> tryGet##FIELD_NAME() const {                                  \
        if (const babelwires::ValueTreeNode* valueTreeNode =                                                           \
                babelwires::InstanceUtils::tryGetChild(this->m_valueTreeNode, #FIELD_NAME)) {                          \
            return {*valueTreeNode};                                                                                   \
        } else {                                                                                                       \
            return {};                                                                                                 \
        }                                                                                                              \
    }                                                                                                                  \
    template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>                                                            \
    std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, babelwires::Instance<VALUE_TYPE>>                            \
        activateAndGet##FIELD_NAME() {                                                                                 \
        return babelwires::InstanceUtils::activateAndGetChild(this->m_valueTreeNode, #FIELD_NAME);                     \
    }                                                                                                                  \
    template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>                                                            \
    std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, void> deactivate##FIELD_NAME() {                             \
        return babelwires::InstanceUtils::deactivateChild(this->m_valueTreeNode, #FIELD_NAME);                         \
    }

/// Add convenience methods for specific tag.
#define DECLARE_INSTANCE_VARIANT_TAG(TAG_NAME)                                                                         \
    bool isTag##TAG_NAME##Selected() const {                                                                           \
        return babelwires::InstanceUtils::getSelectedTag(this->m_valueTreeNode) == #TAG_NAME;                          \
    }                                                                                                                  \
    template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>                                                            \
    std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, void> selectTag##TAG_NAME() {                                \
        return babelwires::InstanceUtils::selectTag(this->m_valueTreeNode, #TAG_NAME);                                 \
    }

/// Declare a (non-optional) array field.
#define DECLARE_INSTANCE_ARRAY_FIELD(FIELD_NAME, ENTRY_TYPE)                                                           \
    babelwires::ArrayInstanceImpl<const babelwires::ValueTreeNode, ENTRY_TYPE> get##FIELD_NAME() const {               \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }                                                                                                                  \
    template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>                                                            \
    std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>,                                                              \
                     babelwires::ArrayInstanceImpl<babelwires::ValueTreeNode, ENTRY_TYPE>>                             \
        get##FIELD_NAME() {                                                                                            \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }

/// Declare a (non-optional) map field.
#define DECLARE_INSTANCE_MAP_FIELD(FIELD_NAME, SOURCE_TYPE, TARGET_TYPE)                                               \
    babelwires::MapInstanceImpl<const babelwires::ValueTreeNode, SOURCE_TYPE, TARGET_TYPE> get##FIELD_NAME() const {   \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }                                                                                                                  \
    template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>                                                            \
    std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>,                                                              \
                     babelwires::MapInstanceImpl<babelwires::ValueTreeNode, SOURCE_TYPE, TARGET_TYPE>>                 \
        get##FIELD_NAME() {                                                                                            \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }

/// Declare a (non-optional) field whose value type is not expected to have an instance specialization.
#define DECLARE_INSTANCE_GENERIC_FIELD(FIELD_NAME)                                                                     \
    babelwires::InstanceUntypedBase<const babelwires::ValueTreeNode> get##FIELD_NAME() const {                         \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }                                                                                                                  \
    template <typename VALUE_TREE_NODE_M = VALUE_TREE_NODE>                                                            \
    std::enable_if_t<!std::is_const_v<VALUE_TREE_NODE_M>, babelwires::InstanceUntypedBase<babelwires::ValueTreeNode>>  \
        get##FIELD_NAME() {                                                                                            \
        return babelwires::InstanceUtils::getChild(this->m_valueTreeNode, #FIELD_NAME);                                \
    }

/// Conclude the declaration.
#define DECLARE_INSTANCE_END()                                                                                         \
    }                                                                                                                  \
    ;                                                                                                                  \
    using Instance = InstanceImpl<babelwires::ValueTreeNode>;                                                          \
    using ConstInstance = InstanceImpl<const babelwires::ValueTreeNode>;
