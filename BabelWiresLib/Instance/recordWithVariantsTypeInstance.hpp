/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Instance/instanceOf.hpp>
#include <BabelWiresLib/Instance/recordTypeInstance.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

namespace babelwires {
    namespace RecordWithVariantsInstanceUtil {
        ShortId getSelectedTag(const ValueFeature& valueFeature);
        void selectTag(ValueFeature& valueFeature, ShortId tag);
    } // namespace RecordWithVariantsInstanceUtil

    template <typename VALUE_FEATURE, typename T>
        requires std::is_base_of_v<RecordWithVariantsType, T>
    class InstanceParent<VALUE_FEATURE, T> : public InstanceCommonBase<VALUE_FEATURE, T> {
      public:
        InstanceParent(VALUE_FEATURE* valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, T>(valueFeature) {}
        ShortId getSelectedTag() const { return RecordWithVariantsInstanceUtil::getSelectedTag(*this->m_valueFeature); }

        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> selectTag(ShortId tag) {
            RecordWithVariantsInstanceUtil::selectTag(*this->m_valueFeature, tag);
        }
    };

} // namespace babelwires
