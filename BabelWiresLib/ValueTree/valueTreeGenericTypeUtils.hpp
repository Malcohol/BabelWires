/**
 * Functions for exploring ValueTrees with Paths.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Path/path.hpp>

#include <optional>
#include <map>

namespace babelwires {
    class ValueTreeNode;
    class TypeRef;

    /// Given a node at a type variable, search up the tree for the GenericType that contains it.
    const ValueTreeNode* tryGetGenericTypeFromVariable(const ValueTreeNode& valueTreeNode);

    /// Check whether a node is between an unresolved type variables and the GenericType that contains it.
    bool containsUnassignedTypeVariable(const ValueTreeNode& valueTreeNode);

    /// Return the maximum level of generic types, above the ValueTreeNode, that any unassigned type variable in the node
    /// references. A return value of -1 means that no unassigned type variables in the ValueTreeNode reference any generic type
    /// above it. The given node is assumed to be maximumPossible levels under a GenericType with an unassigned
    /// variable (i.e. in a well-formed type, the returned value won't exceed the maximumPossible).
    /// To emphasize: If the valueTreeNode holds a GenericType itself, that isn't counted for the result (and shouldn't be
    /// counted in the provided maximumPossible).
    int getMaximumHeightOfUnassignedGenericType(const ValueTreeNode& valueTreeNode, int maximumPossible);

    /// If the target has type variables, determine what types they would get assigned if the source were used to
    /// instantiate them.
    std::optional<std::map<std::tuple<Path, unsigned int>, TypeRef>> getTypeVariableAssignments(const ValueTreeNode& sourceValueTreeNode, const ValueTreeNode& targetValueTreeNode);


} // namespace babelwires
