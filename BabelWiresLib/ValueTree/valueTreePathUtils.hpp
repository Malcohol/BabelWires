/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Path/path.hpp>

namespace babelwires
{
    Path getPathTo(const ValueTreeNode* valueTreeNode);

    template <typename VALUE_TREE_ROOT> struct RootAndPath {
        VALUE_TREE_ROOT& m_root;
        Path m_pathFromRoot;
    };

    RootAndPath<const ValueTreeRoot> getRootAndPathTo(const ValueTreeNode& valueTreeNode);
    RootAndPath<ValueTreeRoot> getRootAndPathTo(ValueTreeNode& valueTreeNode);

    /// Follow the path, returns an error if it cannot be followed.
    ResultT<ValueTreeNode&> followPath(const Path& path, ValueTreeNode& start);

    /// Follow the path, returns an error if it cannot be followed.
    ResultT<const ValueTreeNode&> followPath(const Path& path, const ValueTreeNode& start);

    /// Follow the path, returns nullptr if it cannot be followed.
    ValueTreeNode* tryFollowPath(const Path& path, ValueTreeNode& start);

    /// Follow the path, returns nullptr if it cannot be followed.
    const ValueTreeNode* tryFollowPath(const Path& path, const ValueTreeNode& start);

    /// Follow the path, asserts if it cannot be followed.
    ValueTreeNode& assertFollowPath(const Path& path, ValueTreeNode& start);

    /// Follow the path, asserts if it cannot be followed.
    const ValueTreeNode& assertFollowPath(const Path& path, const ValueTreeNode& start);
}
