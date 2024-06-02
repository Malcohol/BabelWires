/**
 * A set of useful functions for interacting with features of recordType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>

#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

// The following Macros can be used in a record to provide accessors and mutators for fields of standard primitive
// types.

#define FIELD_INT_OPTIONAL(FIELD_NAME)                                                                                 \
    static std::optional<int> tryGet##FIELD_NAME(const babelwires::ValueFeature& recordFeature) {                      \
        if (const auto* childFeature = babelwires::RecordFeatureUtils::tryGetChild(recordFeature, #FIELD_NAME)) {      \
            const auto& intValue = childFeature->getValue()->is<babelwires::IntValue>();                               \
            return intValue.get();                                                                                     \
        }                                                                                                              \
        return {};                                                                                                     \
    }                                                                                                                  \
    static void set##FIELD_NAME(babelwires::ValueFeature& recordFeature, int newValue) {                               \
        auto& childFeature = babelwires::RecordFeatureUtils::activateAndGetChild(recordFeature, #FIELD_NAME);          \
        childFeature.setValue(babelwires::IntValue(newValue));                                                         \
    }

#define FIELD_STRING_OPTIONAL(FIELD_NAME)                                                                              \
    static std::optional<std::string> tryGet##FIELD_NAME(const babelwires::ValueFeature& recordValue) {                \
        if (const auto* childFeature = babelwires::RecordFeatureUtils::tryGetChild(recordValue, #FIELD_NAME)) {        \
            const auto& stringValue = childFeature->getValue()->is<babelwires::StringValue>();                         \
            return stringValue.get();                                                                                  \
        }                                                                                                              \
        return {};                                                                                                     \
    }                                                                                                                  \
    static void set##FIELD_NAME(babelwires::ValueFeature& recordValue, std::string newValue) {                         \
        auto& childFeature = babelwires::RecordFeatureUtils::activateAndGetChild(recordValue, #FIELD_NAME);            \
        childFeature.setValue(babelwires::StringValue(newValue));                                                      \
    }

#define FIELD_ENUM(FIELD_NAME, ENUM_TYPE)                                                                              \
    static ENUM_TYPE::Value get##FIELD_NAME(const babelwires::ValueFeature& recordFeature) {                           \
        const auto& childFeature = babelwires::RecordFeatureUtils::getChild(recordFeature, #FIELD_NAME);               \
        const babelwires::EnumValue& enumValue = childFeature.getValue()->is<babelwires::EnumValue>();                 \
        const ENUM_TYPE& enumType = childFeature.getType().is<ENUM_TYPE>();                                            \
        return enumType.getValueFromIdentifier(enumValue.get());                                                       \
    }                                                                                                                  \
    static void set##FIELD_NAME(babelwires::ValueFeature& recordFeature, ENUM_TYPE::Value newValue) {                  \
        auto& childFeature = babelwires::RecordFeatureUtils::getChild(recordFeature, #FIELD_NAME);                     \
        const ENUM_TYPE& enumType = childFeature.getType().is<ENUM_TYPE>();                                            \
        childFeature.setValue(babelwires::EnumValue(enumType.getIdentifierFromValue(newValue)));                       \
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
