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

    template <typename VALUE_TREE_NODE> struct NodeAndPath {
        VALUE_TREE_NODE& m_node;
        Path m_pathFromNode;
    };

    NodeAndPath<const ValueTreeRoot> getRootAndPathTo(const ValueTreeNode& valueTreeNode);
    NodeAndPath<ValueTreeRoot> getRootAndPathTo(ValueTreeNode& valueTreeNode);

    ValueTreeNode& followPath(const Path& path, ValueTreeNode& start);
    const ValueTreeNode& followPath(const Path& path, const ValueTreeNode& start);

    ValueTreeNode* tryFollowPath(const Path& path, ValueTreeNode& start);
    const ValueTreeNode* tryFollowPath(const Path& path, const ValueTreeNode& start);

    /// Given a node at a type variable, search up the tree for the GenericType that contains it.
    NodeAndPath<ValueTreeNode> getGenericTypeFromVariable(ValueTreeNode& valueTreeNode);
}
