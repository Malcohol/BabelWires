/**
 * Out-of-line utility functions used by instance methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

namespace babelwires {
    /// Out-of-line utility functions used by instance methods.
    namespace InstanceUtils {
        // Utility functions common between RecordTypes and RecordWithVariantTypes

        const babelwires::ValueTreeNode& getChild(const babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);
        babelwires::ValueTreeNode& getChild(babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);

        // Utility functions for RecordTypes

        const babelwires::ValueTreeNode* tryGetChild(const babelwires::ValueTreeNode& recordTreeNode,
                                                    babelwires::ShortId id);
        babelwires::ValueTreeNode& activateAndGetChild(babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);
        void deactivateChild(babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);

        // Utility functions for RecordWithVariantTypes

        ShortId getSelectedTag(const ValueTreeNode& valueTreeNode);
        void selectTag(ValueTreeNode& valueTreeNode, ShortId tag);

        // Utility functions for ArrayTypes

        unsigned int getArraySize(const ValueTreeNode& arrayTreeNode);
        void setArraySize(ValueTreeNode& arrayTreeNode, unsigned int newSize);

        const ValueTreeNode& getChild(const ValueTreeNode& arrayTreeNode, unsigned int index);
        ValueTreeNode& getChild(ValueTreeNode& arrayTreeNode, unsigned int index);
    } // namespace InstanceUtils

} // namespace babelwires
