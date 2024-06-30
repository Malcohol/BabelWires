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

#include <optional>

namespace babelwires {

    namespace RecordFeatureUtils {
        const babelwires::ValueFeature& getChild(const babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
        babelwires::ValueFeature& getChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
        const babelwires::ValueFeature* tryGetChild(const babelwires::ValueFeature& recordFeature,
                                                    babelwires::ShortId id);
        babelwires::ValueFeature& activateAndGetChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
    } // namespace RecordFeatureUtils
} // namespace babelwires
