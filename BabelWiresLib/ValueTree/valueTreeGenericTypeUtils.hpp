/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

namespace babelwires
{
    /// Given a node at a type variable, search up the tree for the GenericType that contains it.
    const ValueTreeNode* tryGetGenericTypeFromVariable(const ValueTreeNode& valueTreeNode);

    /// Check whether a node is between an unresolved type variables and the GenericType that contains it.
    bool containsUnresolvedTypeVariable(const ValueTreeNode& valueTreeNode);
}
