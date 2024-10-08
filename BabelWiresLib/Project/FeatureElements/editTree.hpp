/**
 * The EditTree arranges edits (modifiers and expand/collapse) in a tree organized by paths.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <Common/types.hpp>

#include <memory>
#include <type_traits>

namespace babelwires {
    class Modifier;
    class ConnectionModifier;

    /// Arranges edits (modifiers and expand/collapse) in a tree organized by paths.
    /// All tree data is stored in one compact block, so it should be cheap to traverse assuming EditTrees are small.
    /// Management of the changes of the expansion state is done explicitly within the tree.
    class EditTree {
      public:
        virtual ~EditTree();

        void addModifier(std::unique_ptr<Modifier> modifier);
        std::unique_ptr<Modifier> removeModifier(const Modifier* modifier);
        Modifier* findModifier(const FeaturePath& featurePath);
        const Modifier* findModifier(const FeaturePath& featurePath) const;
        bool isExpanded(const FeaturePath& featurePath) const;

        void setExpanded(const FeaturePath& featurePath, bool expanded);

        /// The feature at the path is not collapsible, so it should be treated as expanded without an edit.
        void setImplicitlyExpanded(const FeaturePath& featurePath, bool expanded);

        /// Adjust edits which refer to an array at the path, starting at the startIndex.
        void adjustArrayIndices(const FeaturePath& pathToArray, ArrayIndex startIndex, int adjustment);

        /// Clear changes to the modifiers and the expansion state.
        void clearChanges();

        /// Used by truncatePathAtFirstCollapsedNode to determine whether truncation should be
        /// considered in the current state, or in the state before the current changes.
        enum class State {
            /// Truncation should be performed in the state currently described by the tree.
            CurrentState,
            /// Truncation should be performed in the state described by the tree, before changes.
            PreviousState
        };

        /// Remove the tail of the path hidden within a collapsed compound feature.
        void truncatePathAtFirstCollapsedNode(FeaturePath& path, State state) const;

        /// Return all the explicitly expanded paths in the tree.
        std::vector<FeaturePath> getAllExplicitlyExpandedPaths(const FeaturePath& path = FeaturePath()) const;

      public:
        // Iteration through the tree of nodes.

        template <typename EDIT_TREE> struct Iterator;

        Iterator<EditTree> begin();
        Iterator<const EditTree> begin() const;

        Iterator<EditTree> end();
        Iterator<const EditTree> end() const;

        // Iteration over just modifiers

        /// Iterates through the tree or a subtree in path order.
        /// Will only return modifiers of the MODIFIER_TYPE, where MODIFIER_TYPE must be either Modifier or
        /// ConnectionModifier.
        template <typename EDIT_TREE, typename MODIFIER_TYPE> struct ModifierIterator;

        /// Provides begin and end iterators of the specified type.
        template <typename ITERATOR> struct IteratorRange;

        /// Allows iteration over the whole tree.
        template <typename MODIFIER_TYPE = Modifier>
        IteratorRange<ModifierIterator<EditTree, MODIFIER_TYPE>> modifierRange();
        template <typename MODIFIER_TYPE = Modifier>
        IteratorRange<ModifierIterator<const EditTree, MODIFIER_TYPE>> modifierRange() const;

        /// Allows iteration over the subtree at the path (which must be in the tree).
        template <typename MODIFIER_TYPE = Modifier>
        IteratorRange<ModifierIterator<EditTree, MODIFIER_TYPE>> modifierRange(const FeaturePath& featurePath);
        template <typename MODIFIER_TYPE = Modifier>
        IteratorRange<ModifierIterator<const EditTree, MODIFIER_TYPE>>
        modifierRange(const FeaturePath& featurePath) const;

      private:
        struct RootedPath;
        struct RootedPathIterator;

        struct FindNodeIndexResult {
            int m_nodeIndex = -1;
            /// If there is a missing child, this is the index where the missing child would be.
            int m_missingChildIndex = -1;
        };

        /// A stack of indices which can be returned by findNodeIndex.
        using AncestorStack = std::vector<std::uint16_t>;

        /// Advance current along the nodes and return the index of the last node reached.
        /// If ancestorStackOut is provided, the indices of the path will be pushed on.
        FindNodeIndexResult findNodeIndex(RootedPathIterator& current, const RootedPathIterator& end,
                                          AncestorStack* ancestorStackOut = nullptr) const;

        /// Check assertions about the validity of the tree.
        bool validateTree() const;

        template <typename EDIT_TREE, typename MODIFIER_TYPE> friend struct ModifierIterator;
        template <typename EDIT_TREE, typename MODIFIER_TYPE> friend struct ModifierRange;

        struct TreeNode;

        using EditNodeFunc = std::function<void(TreeNode& node)>;

        /// Apply the edit to the node at the path, allowing nodes to be added.
        void addEdit(const FeaturePath& featurePath, const EditNodeFunc& applyFunc);

        /// Remove the edit from the node at the path, allowing nodes to be removed.
        void removeEdit(const FeaturePath& featurePath, const EditNodeFunc& applyFunc);

        using TreeNodeIndex = std::uint16_t;

        /// Get the path of steps which leads to the node at the given index.
        FeaturePath getPathToNode(TreeNodeIndex nodeIndex) const;

      public:
        /// This determines if the second or deeper levels are expanded by default.
        /// The first level is always expanded.
        static constexpr bool c_expandedByDefault = false;

      private:
        /// The data structure which carries the tree's data.
        struct TreeNode {
            /// Non-null for leaf nodes.
            std::unique_ptr<Modifier> m_modifier;
            /// The step to this node from its parent.
            PathStep m_step;
            /// The total number of descendents of this node (not including this node).
            std::uint16_t m_numDescendents = 0;

            /// Is the feature at this path expanded? (Compounds only.)
            bool m_isExpanded = c_expandedByDefault;

            /// True after m_isExpanded is changed, until clearChanges is called.
            bool m_isExpandedChanged = false;

            /// The feature at the path is not collapsible, so it should be treated as expanded without an edit.
            bool m_isImplicitlyExpanded = false;

            /// Is this tree node needed, either because it has descendents or it
            /// carries an edit?
            bool isNeeded() const;
        };

        /// Children follow immediately after their parent.
        /// This is empty when there are no edits.
        std::vector<TreeNode> m_nodes;
    };

} // namespace babelwires

#include <BabelWiresLib/Project/FeatureElements/editTree_inl.hpp>