/**
 * A set of useful functions for interacting with features of ArrayType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/featureWrapper.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#define ARRAY_FEATURE_WRAPPER(ELEMENT_TYPE) \
    using ElementTypeForFeatureWrapper = ELEMENT_TYPE;

namespace babelwires {
    class ValueFeature;

    /// A set of useful functions for interacting with features of ArrayType.
    namespace ArrayFeatureUtils {
        unsigned int getArraySize(const ValueFeature& arrayFeature);
        void setArraySize(ValueFeature& arrayFeature, unsigned int newSize);

        const ValueFeature& getChild(const ValueFeature& arrayFeature, unsigned int index);
        ValueFeature& getChild(ValueFeature& arrayFeature, unsigned int index);
    }; // namespace ArrayFeatureUtils

    template <typename VALUE_FEATURE, typename T>
        requires std::is_base_of_v<ArrayType, T>
    class FeatureWrapper<VALUE_FEATURE, T> {
        VALUE_FEATURE* m_valueFeature;

      public:
        FeatureWrapper(VALUE_FEATURE* valueFeature)
            : m_valueFeature(valueFeature) {
            assert(!valueFeature || valueFeature->getType().template as<ArrayType>());
        }

        unsigned int getSize() const {
            assert(m_valueFeature);
            return ArrayFeatureUtils::getArraySize(*m_valueFeature);
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> setSize(unsigned int newSize) {
            assert(m_valueFeature);
            ArrayFeatureUtils::setArraySize(*m_valueFeature, newSize);
        }
        FeatureWrapper<const ValueFeature, typename T::ElementTypeForFeatureWrapper> getEntry(unsigned int index) const {
            assert(m_valueFeature);
            return &ArrayFeatureUtils::getChild(*m_valueFeature, index);
        }
        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>,
                         FeatureWrapper<VALUE_FEATURE, typename T::ElementTypeForFeatureWrapper>>
            getEntry(unsigned int index) {
            return &ArrayFeatureUtils::getChild(*m_valueFeature, index);
        }
    };
} // namespace babelwires
