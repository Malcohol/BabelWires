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
    DECLARE_INSTANCE_BEGIN_WITH_PARENT(                                                                                 \
        TYPE, babelwires::RecordWithVariantsInstanceBase<VALUE_FEATURE MACRO_COMMA TYPE>)

#define RECORD_WITH_VARIANTS_END() DECLARE_INSTANCE_END()

namespace babelwires {
    namespace RecordWithVariantsInstanceUtil {
        ShortId getSelectedTag(const ValueFeature& valueFeature);
        void selectTag(ValueFeature& valueFeature, ShortId tag);
    }

    template <typename VALUE_FEATURE, typename VALUE_TYPE>
    class RecordWithVariantsInstanceBase : public InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE> {
      public:
        RecordWithVariantsInstanceBase(VALUE_FEATURE* valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, VALUE_TYPE>(valueFeature) {}
        ShortId getSelectedTag() const { return RecordWithVariantsInstanceUtil::getSelectedTag(*this->m_valueFeature); }

        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> selectTag(ShortId tag) {
            RecordWithVariantsInstanceUtil::selectTag(*this->m_valueFeature, tag);
        }
    };


} // namespace babelwires