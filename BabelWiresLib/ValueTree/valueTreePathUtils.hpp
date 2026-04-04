/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Path/path.hpp>

namespace babelwires
{
    BABELWIRESLIB_API Path getPathTo(const ValueTreeNode* valueTreeNode);

    template <typename VALUE_TREE_ROOT> struct RootAndPath {
        VALUE_TREE_ROOT& m_root;
        Path m_pathFromRoot;
    };

    BABELWIRESLIB_API RootAndPath<const ValueTreeRoot> getRootAndPathTo(const ValueTreeNode& valueTreeNode);
    BABELWIRESLIB_API RootAndPath<ValueTreeRoot> getRootAndPathTo(ValueTreeNode& valueTreeNode);

    /// Follow the path, returns an error if it cannot be followed.
    BABELWIRESLIB_API ResultT<ValueTreeNode&> followPath(const Path& path, ValueTreeNode& start);

    /// Follow the path, returns an error if it cannot be followed.
    BABELWIRESLIB_API ResultT<const ValueTreeNode&> followPath(const Path& path, const ValueTreeNode& start);

    /// Follow the path, returns nullptr if it cannot be followed.
    BABELWIRESLIB_API ValueTreeNode* tryFollowPath(const Path& path, ValueTreeNode& start);

    /// Follow the path, returns nullptr if it cannot be followed.
    BABELWIRESLIB_API const ValueTreeNode* tryFollowPath(const Path& path, const ValueTreeNode& start);

    /// Follow the path, asserts if it cannot be followed.
    BABELWIRESLIB_API ValueTreeNode& assertFollowPath(const Path& path, ValueTreeNode& start);

    /// Follow the path, asserts if it cannot be followed.
    BABELWIRESLIB_API const ValueTreeNode& assertFollowPath(const Path& path, const ValueTreeNode& start);
}
