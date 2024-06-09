/**
 * A set of useful functions for interacting with features of ArrayType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    class ValueFeature;

    /// A set of useful functions for interacting with features of ArrayType.
    namespace ArrayFeatureUtils {
        unsigned int getArraySize(const ValueFeature& arrayFeature);
        void setArraySize(ValueFeature& arrayFeature, unsigned int newSize);

        const ValueFeature& getChild(const ValueFeature& arrayFeature,
                                                        unsigned int index);
        ValueFeature& getChild(ValueFeature& arrayFeature, unsigned int index);
    }; // namespace ArrayFeatureUtils
} // namespace babelwires
