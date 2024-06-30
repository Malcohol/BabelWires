/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>

#include <optional>

#define FEATURE_WRAPPER_FIELD(FIELD_NAME, VALUE_TYPE)                                                                  \
    babelwires::Instance<const babelwires::ValueFeature, VALUE_TYPE> get##FIELD_NAME() const {                   \
        return &babelwires::RecordFeatureUtils::getChild(*this->m_valueFeature, #FIELD_NAME);                          \
    }                                                                                                                  \
    template <typename VALUE_FEATURE_M = VALUE_FEATURE>                                                                \
    std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>,                                                                \
                     babelwires::Instance<babelwires::ValueFeature, VALUE_TYPE>>                                 \
        get##FIELD_NAME() {                                                                                            \
        return &babelwires::RecordFeatureUtils::getChild(*this->m_valueFeature, #FIELD_NAME);                          \
    }

#define FEATURE_WRAPPER_FIELD_OPTIONAL(FIELD_NAME, VALUE_TYPE)                                                         \
    FEATURE_WRAPPER_FIELD(FIELD_NAME, VALUE_TYPE)                                                                      \
    std::optional<babelwires::Instance<const babelwires::ValueFeature, VALUE_TYPE>> tryGet##FIELD_NAME() const { \
        if (const babelwires::ValueFeature* valueFeature =                                                             \
                babelwires::RecordFeatureUtils::tryGetChild(*this->m_valueFeature, #FIELD_NAME)) {                     \
            return {valueFeature};                                                                                     \
        } else {                                                                                                       \
            return {};                                                                                                 \
        }                                                                                                              \
    }                                                                                                                  \
    template <typename VALUE_FEATURE_M = VALUE_FEATURE>                                                                \
    std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>,                                                                \
                     babelwires::Instance<babelwires::ValueFeature, VALUE_TYPE>>                                 \
        activateAndGet##FIELD_NAME() {                                                                                 \
        return &babelwires::RecordFeatureUtils::activateAndGetChild(*this->m_valueFeature, #FIELD_NAME);               \
    }


namespace babelwires {

    namespace RecordFeatureUtils {
        const babelwires::ValueFeature& getChild(const babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
        babelwires::ValueFeature& getChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
        const babelwires::ValueFeature* tryGetChild(const babelwires::ValueFeature& recordFeature,
                                                    babelwires::ShortId id);
        babelwires::ValueFeature& activateAndGetChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
    } // namespace RecordFeatureUtils
} // namespace babelwires