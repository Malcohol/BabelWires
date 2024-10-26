/**
 * Out-of-line utility functions used by instance methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueTreeNode.hpp>

namespace babelwires {
    /// Out-of-line utility functions used by instance methods.
    namespace InstanceUtils {
        // Utility functions common between RecordTypes and RecordWithVariantTypes

        const babelwires::ValueTreeNode& getChild(const babelwires::ValueTreeNode& recordFeature, babelwires::ShortId id);
        babelwires::ValueTreeNode& getChild(babelwires::ValueTreeNode& recordFeature, babelwires::ShortId id);

        // Utility functions for RecordTypes

        const babelwires::ValueTreeNode* tryGetChild(const babelwires::ValueTreeNode& recordFeature,
                                                    babelwires::ShortId id);
        babelwires::ValueTreeNode& activateAndGetChild(babelwires::ValueTreeNode& recordFeature, babelwires::ShortId id);
        void deactivateChild(babelwires::ValueTreeNode& recordFeature, babelwires::ShortId id);

        // Utility functions for RecordWithVariantTypes

        ShortId getSelectedTag(const ValueTreeNode& valueFeature);
        void selectTag(ValueTreeNode& valueFeature, ShortId tag);

        // Utility functions for ArrayTypes

        unsigned int getArraySize(const ValueTreeNode& arrayFeature);
        void setArraySize(ValueTreeNode& arrayFeature, unsigned int newSize);

        const ValueTreeNode& getChild(const ValueTreeNode& arrayFeature, unsigned int index);
        ValueTreeNode& getChild(ValueTreeNode& arrayFeature, unsigned int index);
    } // namespace InstanceUtils

} // namespace babelwires
