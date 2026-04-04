/**
 * Out-of-line utility functions used by instance methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <BaseLib/Result/result.hpp>

namespace babelwires {
    /// Out-of-line utility functions used by instance methods.
    namespace InstanceUtils {
        // Utility functions common between RecordTypes and RecordWithVariantTypes

        BABELWIRESLIB_API const babelwires::ValueTreeNode& getChild(const babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);
        BABELWIRESLIB_API babelwires::ValueTreeNode& getChild(babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);

        // Utility functions for RecordTypes

        BABELWIRESLIB_API const babelwires::ValueTreeNode* tryGetChild(const babelwires::ValueTreeNode& recordTreeNode,
                                babelwires::ShortId id);
        BABELWIRESLIB_API babelwires::ValueTreeNode& activateAndGetChild(babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);
        BABELWIRESLIB_API void deactivateChild(babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id);

        // Utility functions for RecordWithVariantTypes

        BABELWIRESLIB_API ShortId getSelectedTag(const ValueTreeNode& valueTreeNode);
        BABELWIRESLIB_API void selectTag(ValueTreeNode& valueTreeNode, ShortId tag);

        // Utility functions for ArrayTypes

        BABELWIRESLIB_API unsigned int getArraySize(const ValueTreeNode& arrayTreeNode);
        BABELWIRESLIB_API Result setArraySize(ValueTreeNode& arrayTreeNode, unsigned int newSize);
        BABELWIRESLIB_API void assertSetArraySize(ValueTreeNode& arrayTreeNode, unsigned int newSize);

        BABELWIRESLIB_API const ValueTreeNode& getChild(const ValueTreeNode& arrayTreeNode, unsigned int index);
        BABELWIRESLIB_API ValueTreeNode& getChild(ValueTreeNode& arrayTreeNode, unsigned int index);
    } // namespace InstanceUtils

} // namespace babelwires
