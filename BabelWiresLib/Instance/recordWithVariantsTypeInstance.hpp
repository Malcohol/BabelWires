/**
 * Provides some standard methods for instances of RecordForVariantType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Instance/instanceTemplates.hpp>
#include <BabelWiresLib/Instance/instanceUtils.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

namespace babelwires {
    /// Provides some standard methods for instances of RecordForVariantType.
    template <typename VALUE_FEATURE, typename T>
        requires std::is_base_of_v<RecordWithVariantsType, T>
    class InstanceParent<VALUE_FEATURE, T> : public InstanceCommonBase<VALUE_FEATURE, T> {
      public:
        InstanceParent(VALUE_FEATURE& valueFeature)
            : InstanceCommonBase<VALUE_FEATURE, T>(valueFeature) {}
        ShortId getSelectedTag() const { return InstanceUtils::getSelectedTag(this->m_valueTreeNode); }

        template <typename VALUE_FEATURE_M = VALUE_FEATURE>
        std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>, void> selectTag(ShortId tag) {
            InstanceUtils::selectTag(this->m_valueTreeNode, tag);
        }
    };

} // namespace babelwires
