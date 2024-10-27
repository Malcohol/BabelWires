/**
 * Specialized instance handling for ArrayTypes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Instance/instanceTemplates.hpp>
#include <BabelWiresLib/Instance/instanceUtils.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

namespace babelwires {
    /// It's useful to have instances for array types even when they do not have a dedicated C++ class.
    /// For example, if they are defined using a type constructor.
    /// The DSL provides a DECLARE_INSTANCE_ARRAY_FIELD macro which directly uses the following instance template.
    /// Note: The second type parameter is the entry type, not array type.
    template <typename VALUE_FEATURE, typename ENTRY_TYPE>
    class ArrayInstanceImpl : public InstanceCommonBase<VALUE_FEATURE, ArrayType> {
      public:
        ArrayInstanceImpl(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, ArrayType>(valueFeature) {}

        unsigned int getSize() const { return InstanceUtils::getArraySize(this->m_valueTreeNode); }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> setSize(unsigned int newSize) {
            InstanceUtils::setArraySize(this->m_valueTreeNode, newSize);
        }
        ConstInstance<ENTRY_TYPE> getEntry(unsigned int index) const {
            return InstanceUtils::getChild(this->m_valueTreeNode, index);
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, Instance<ENTRY_TYPE>> getEntry(unsigned int index) {
            return InstanceUtils::getChild(this->m_valueTreeNode, index);
        }
    };
} // namespace babelwires
