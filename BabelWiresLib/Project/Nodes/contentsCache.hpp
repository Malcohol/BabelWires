/**
 * The ContentsCache summarizes the contents of a Node visible to the user.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <BabelWiresLib/Path/path.hpp>
#include <Common/Utilities/enumFlags.hpp>

namespace babelwires {

    class ValueTreeNode;
    class EditTree;
    class ContentsCache;

    namespace Detail {
        struct ContentsCacheBuilder;
    }

    /// The information cached about a single row in the contents of a Node.
    class ContentsCacheEntry {
      public:
        ContentsCacheEntry(std::string label, const ValueTreeNode* input, const ValueTreeNode* output,
                           const Path& path, std::uint8_t depth, std::uint8_t indent);

        const std::string& getLabel() const { return m_label; }
        const ValueTreeNode* getInput() const { return m_input; }
        const ValueTreeNode* getOutput() const { return m_output; }
        const Path& getPath() const { return m_path; }

        /// Get the indent of this row.
        std::uint8_t getIndent() const { return m_indent; }

        /// Get the number of levels of nesting.
        std::uint8_t getDepth() const { return m_depth; }

        bool isExpandable() const { return m_isExpandable; }
        bool isExpanded() const { return m_isExpanded; }
        bool hasModifier() const { return m_hasModifier; }
        bool isStructureEditable() const { return m_isStructureEditable; }
        bool hasFailedModifier() const { return m_hasFailedModifier; }
        bool hasHiddenModifier() const { return m_hasHiddenModifiers; }
        bool hasFailedHiddenModifiers() const { return m_hasFailedHiddenModifiers; }
        bool hasSubmodifiers() const { return m_hasSubModifiers; }
        
        /// This entry is strictly between an unassigned input type variable and its generic type.
        bool hasUnassignedInputTypeVariable() const { return m_hasUnassignedInputTypeVariable; }

        /// This entry is strictly between an unassigned output type variable and its generic type.
        bool hasUnassignedOutputTypeVariable() const { return m_hasUnassignedOutputTypeVariable; }

        /// Convenience method which also returns true for unassigned type variables.
        bool isOrHasUnassignedInputTypeVariable() const;

        /// Convenience method which also returns true for unassigned type variables.
        bool isOrHasUnassignedOutputTypeVariable() const;

        const ValueTreeNode* getInputThenOutput() const { return m_input ? m_input : m_output; }

        const ValueTreeNode* getOutputThenInput() const { return m_output ? m_output : m_input; }

      private:
        friend ContentsCache;
        friend Detail::ContentsCacheBuilder;

        std::string m_label;
        const ValueTreeNode* m_input;
        const ValueTreeNode* m_output;

        /// A path which is common to both input and output features.
        const Path m_path;

        /// The number of levels of nesting.
        // TODO Isn't this just m_path.getNumSteps()?
        std::uint8_t m_depth;

        /// The number of apparent levels of nesting, accounting for inlining.
        std::uint8_t m_indent;

        /// Does the underlying feature have child features?
        /// Note: It's difficult to calculate the number of children without exploring them
        /// because of the rules about sharing paths between input/output features.
        bool m_isExpandable : 1;

        /// If this is a compound, is it expanded.
        bool m_isExpanded : 1;

        /// Is this entry modified?
        bool m_hasModifier : 1;

        /// Is it permitted to edit the structure of this row (if compound).
        /// False if this row is at or beneath the target of a connection modifier.
        bool m_isStructureEditable : 1;

        /// Has that modifier failed?
        bool m_hasFailedModifier : 1;

        /// Are there modifiers beneath this entry which are not visible?
        bool m_hasHiddenModifiers : 1;

        /// Are there modifiers beneath this entry which are not visible and which failed?
        bool m_hasFailedHiddenModifiers : 1;

        /// Are there any modifiers at or beneath this entry?
        bool m_hasSubModifiers : 1;

        /// Does the input type of this entry contain an unresolved type variable?
        bool m_hasUnassignedInputTypeVariable : 1;

        /// Does the output type of this entry contain an unresolved type variable?
        bool m_hasUnassignedOutputTypeVariable : 1;
    };

    /// A cache of the contents of a Node visible to the user.
    /// This is relevant only to the UI, but is included in the base library
    /// because the logic is independent of the particular UI framework.
    class ContentsCache {
      public:
        /// The EditTree object is never replaced, so we can keep a reference.
        /// (The input/output features can change (e.g. after a reload) so we
        /// can't store them in this object.)
        /// Non-const because of features with a not collapsable style.
        ContentsCache(EditTree& edits);

        /// Build the cache with the given input and output features.
        void setValueTrees(std::string rootName, const ValueTreeNode* input, const ValueTreeNode* output);

        /// Update the part of the cache concerning modifiers.
        void updateModifierCache();

        /// Get the number of visible rows.
        int getNumRows() const;

        /// Get the ith visible row.
        const ContentsCacheEntry* getEntry(int i) const;

        /// Return the index if the path is in one of the visible rows.
        /// Otherwise return -1.
        int getIndexOfPath(bool seekInputFeature, const Path& path) const;

        /// Describes the way a node may have changed.
        enum class Changes : unsigned int {
            NothingChanged = 0b0,

            StructureChanged = 0b1,

            SomethingChanged = 0b1
        };

        /// Query the Node for changes.
        bool isChanged(Changes changes) const;

        /// Clear any changes the node is carrying.
        void clearChanges();

      private:
        /// Set flags recording a change.
        void setChanged(Changes changes);

        /// Update the flags for modifiers in the entries.
        void updateModifierFlags();

      private:
        /// The rows of the contents. The first row may be hidden if there's no useful information there.
        std::vector<ContentsCacheEntry> m_rows;

        /// The edits carries the information about which nodes have been expanded.
        /// Non-const because the cache builder can encounter features whose style states that they are not collapsable
        /// (i.e. that they are expanded without required an edit) and we have to update the edit tree with that fact.
        EditTree& m_edits;

        Changes m_changes;
    };

    DEFINE_ENUM_FLAG_OPERATORS(ContentsCache::Changes);

} // namespace babelwires