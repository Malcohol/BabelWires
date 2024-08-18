/**
 * Out-of-line utility functions used by instance methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>

namespace babelwires {
    /// Out-of-line utility functions used by instance methods.
    namespace InstanceUtils {
        // Utility functions common between RecordTypes and RecordWithVariantTypes

        const babelwires::ValueFeature& getChild(const babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
        babelwires::ValueFeature& getChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id);

        // Utility functions for RecordTypes

        const babelwires::ValueFeature* tryGetChild(const babelwires::ValueFeature& recordFeature,
                                                    babelwires::ShortId id);
        babelwires::ValueFeature& activateAndGetChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id);
        void deactivateChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id);

        // Utility functions for RecordWithVariantTypes

        ShortId getSelectedTag(const ValueFeature& valueFeature);
        void selectTag(ValueFeature& valueFeature, ShortId tag);

        // Utility functions for ArrayTypes

        unsigned int getArraySize(const ValueFeature& arrayFeature);
        void setArraySize(ValueFeature& arrayFeature, unsigned int newSize);

        const ValueFeature& getChild(const ValueFeature& arrayFeature, unsigned int index);
        ValueFeature& getChild(ValueFeature& arrayFeature, unsigned int index);
    } // namespace InstanceUtils

} // namespace babelwires
