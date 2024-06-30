/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Instance/recordTypeInstance.hpp>
#include <BabelWiresLib/Instance/instanceOf.hpp>

#define RECORD_WITH_VARIANTS_BEGIN(TYPE)                                                                               \
    FEATURE_WRAPPER_BEGIN_WITH_PARENT(                                                                                 \
        TYPE, babelwires::RecordWithVariantsFeatureWrapperBase<VALUE_FEATURE MACRO_COMMA TYPE>)

#define RECORD_WITH_VARIANTS_END() FEATURE_WRAPPER_END()

namespace babelwires {
    namespace RecordWithVariantsFeatureWrapperUtil {
        ShortId getSelectedTag(const ValueFeature& valueFeature);
        void selectTag(ValueFeature& valueFeature, ShortId tag);
    }

    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class RecordWithVariantsFeatureWrapperBase : public FeatureWrapperCommonBase<VALUE_FEATURE, VALUE_TYPE> {
      public:
        RecordWithVariantsFeatureWrapperBase(VALUE_FEATURE* valueFeature)
            : FeatureWrapperCommonBase<VALUE_FEATURE, VALUE_TYPE>(valueFeature) {}
        ShortId getSelectedTag() const { return RecordWithVariantsFeatureWrapperUtil::getSelectedTag(*this->m_valueFeature); }

        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> selectTag(ShortId tag) {
            RecordWithVariantsFeatureWrapperUtil::selectTag(*this->m_valueFeature, tag);
        }
    };


} // namespace babelwires