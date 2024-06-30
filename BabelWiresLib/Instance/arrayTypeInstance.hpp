/**
 * A set of useful functions for interacting with features of ArrayType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Instance/instanceTemplates.hpp>
#include <BabelWiresLib/Instance/instanceUtils.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#define DECLARE_ARRAY_INSTANCE(ELEMENT_TYPE) \
    using EntryTypeForInstance = ELEMENT_TYPE;

namespace babelwires {
    class ValueFeature;

    template <typename VALUE_FEATURE, typename T>
        requires std::is_base_of_v<ArrayType, T>
    class Instance<VALUE_FEATURE, T> {
        VALUE_FEATURE* m_valueFeature;

      public:
        Instance(VALUE_FEATURE* valueFeature)
            : m_valueFeature(valueFeature) {
            assert(!valueFeature || valueFeature->getType().template as<ArrayType>());
        }

        unsigned int getSize() const {
            assert(m_valueFeature);
            return InstanceUtils::getArraySize(*m_valueFeature);
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> setSize(unsigned int newSize) {
            assert(m_valueFeature);
            InstanceUtils::setArraySize(*m_valueFeature, newSize);
        }
        Instance<const ValueFeature, typename T::EntryTypeForInstance> getEntry(unsigned int index) const {
            assert(m_valueFeature);
            return &InstanceUtils::getChild(*m_valueFeature, index);
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>,
                         Instance<VALUE_FEATURE, typename T::EntryTypeForInstance>>
            getEntry(unsigned int index) {
            return &InstanceUtils::getChild(*m_valueFeature, index);
        }
    };
} // namespace babelwires
