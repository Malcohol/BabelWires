/**
 * Out-of-line utility functions used by instance methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/feature.hpp>

namespace babelwires {
    /// Out-of-line utility functions used by instance methods.
    namespace InstanceUtils {
        // Utility functions common between RecordTypes and RecordWithVariantTypes

        const babelwires::Feature& getChild(const babelwires::Feature& recordFeature, babelwires::ShortId id);
        babelwires::Feature& getChild(babelwires::Feature& recordFeature, babelwires::ShortId id);

        // Utility functions for RecordTypes

        const babelwires::Feature* tryGetChild(const babelwires::Feature& recordFeature,
                                                    babelwires::ShortId id);
        babelwires::Feature& activateAndGetChild(babelwires::Feature& recordFeature, babelwires::ShortId id);
        void deactivateChild(babelwires::Feature& recordFeature, babelwires::ShortId id);

        // Utility functions for RecordWithVariantTypes

        ShortId getSelectedTag(const Feature& valueFeature);
        void selectTag(Feature& valueFeature, ShortId tag);

        // Utility functions for ArrayTypes

        unsigned int getArraySize(const Feature& arrayFeature);
        void setArraySize(Feature& arrayFeature, unsigned int newSize);

        const Feature& getChild(const Feature& arrayFeature, unsigned int index);
        Feature& getChild(Feature& arrayFeature, unsigned int index);
    } // namespace InstanceUtils

} // namespace babelwires
