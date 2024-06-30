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

// TODO: Currently the array type must declare the class of its entry type. This seems awkward.
#define DECLARE_ARRAY_INSTANCE(ELEMENT_TYPE) \
    using EntryTypeForInstance = ELEMENT_TYPE;

namespace babelwires {
    class ValueFeature;

    /// Specialized instance handling for ArrayTypes.
    template <typename VALUE_FEATURE, typename ARRAY_TYPE>
        requires std::is_base_of_v<ArrayType, ARRAY_TYPE>
    class Instance<VALUE_FEATURE, ARRAY_TYPE> : public InstanceCommonBase<VALUE_FEATURE, ARRAY_TYPE> {
      public:
        Instance(VALUE_FEATURE* valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, ARRAY_TYPE>(valueFeature) {}

        unsigned int getSize() const {
            assert(this->m_valueFeature);
            return InstanceUtils::getArraySize(*this->m_valueFeature);
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> setSize(unsigned int newSize) {
            assert(this->m_valueFeature);
            InstanceUtils::setArraySize(*this->m_valueFeature, newSize);
        }
        Instance<const ValueFeature, typename ARRAY_TYPE::EntryTypeForInstance> getEntry(unsigned int index) const {
            assert(this->m_valueFeature);
            return &InstanceUtils::getChild(*this->m_valueFeature, index);
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>,
                         Instance<VALUE_FEATURE, typename ARRAY_TYPE::EntryTypeForInstance>>
            getEntry(unsigned int index) {
            return &InstanceUtils::getChild(*this->m_valueFeature, index);
        }
    };
} // namespace babelwires
