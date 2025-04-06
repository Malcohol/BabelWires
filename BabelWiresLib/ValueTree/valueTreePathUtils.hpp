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

    ValueTreeNode& followPath(const Path& path, ValueTreeNode& start);
    const ValueTreeNode& follow(const Path& path, const ValueTreeNode& start);
    ValueTreeNode* tryFollow(const Path& path, ValueTreeNode& start);
    const ValueTreeNode* tryFollow(const Path& path, const ValueTreeNode& start);
}
