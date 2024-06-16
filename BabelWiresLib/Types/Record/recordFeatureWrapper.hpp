/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#define FEATURE_WRAPPER_FIELD(FIELD_NAME, VALUE_TYPE)                                                                  \
    babelwires::FeatureWrapper<const babelwires::ValueFeature, VALUE_TYPE> get##FIELD_NAME() const {                   \
        return babelwires::RecordFeatureUtils::getChild(m_valueFeature, #FIELD_NAME);                                  \
    }                                                                                                                  \
    template <typename VALUE_FEATURE_M = VALUE_FEATURE>                                                                \
    std::enable_if_t<!std::is_const_v<VALUE_FEATURE_M>,                                                                \
                     babelwires::FeatureWrapper<babelwires::ValueFeature, VALUE_TYPE>>                                 \
        get##FIELD_NAME() {                                                                                            \
        return babelwires::RecordFeatureUtils::getChild(m_valueFeature, #FIELD_NAME);                                  \
    }
