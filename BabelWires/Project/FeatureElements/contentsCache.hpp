/**
 * The ContentsCache summarizes the contents of a FeatureElement visible to the user.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "BabelWires/Features/Path/featurePath.hpp"
#include "Common/Utilities/enumFlags.hpp"

namespace babelwires {

    class Feature;
    class CompoundFeature;
    class RecordFeature;
    class ArrayFeature;
    class EditTree;

    /// The information cached about a single row in the contents of a feature element.
    struct ContentsCacheEntry {
        ContentsCacheEntry(std::string label, const Feature* inputFeature, const Feature* outputFeature,
                           const FeaturePath& path, std::uint8_t parentIndex);

        const std::string& getLabel() const { return m_label; }
        const Feature* getInputFeature() const { return m_inputFeature; }
        const Feature* getOutputFeature() const { return m_outputFeature; }
        const FeaturePath& getPath() const { return m_path; }
        bool isExpandable() const { return m_isExpandable; }
        bool isExpanded() const { return m_isExpanded; }
        bool hasModifier() const { return m_hasModifier; }
        bool hasFailedModifier() const { return m_hasFailedModifier; }
        bool hasHiddenModifier() const { return m_hasHiddenModifiers; }
        bool hasFailedHiddenModifiers() const { return m_hasFailedHiddenModifiers; }
        bool hasSubmodifiers() const { return m_hasSubModifiers; }

        const Feature* getInputThenOutputFeature() const { return m_inputFeature ? m_inputFeature : m_outputFeature; }

        const Feature* getOutputThenInputFeature() const { return m_outputFeature ? m_outputFeature : m_inputFeature; }

        std::string m_label;
        const Feature* m_inputFeature;
        const Feature* m_outputFeature;

        /// A path which is common to both input and output features.
        const FeaturePath m_path;

        /// Does the underlying feature have child features?
        /// Note: It's difficult to calculate the number of children without exploring them
        /// because of the rules about sharing paths between input/output features.
        bool m_isExpandable = false;

        /// If this is a compound, is it expanded.
        bool m_isExpanded = false;

        /// Is this entry modified?
        bool m_hasModifier = false;

        /// Has that modifier failed?
        bool m_hasFailedModifier = false;

        /// Are there modifiers beneath this entry which are not visible?
        bool m_hasHiddenModifiers = false;

        /// Are there modifiers beneath this entry which are not visible and which failed?
        bool m_hasFailedHiddenModifiers = false;

        /// Are there any modifiers at or beneath this entry?
        bool m_hasSubModifiers = false;

        /// A cache may not contain this many elements.
        static constexpr std::uint8_t c_invalidIndex = 255;

        /// The index of this entries parent in the cache.
        // TODO Not currently used and imposes a technical limit. Remove?
        std::uint8_t m_parentIndex = c_invalidIndex;
    };

    /// A cache of the contents of a feature element visible to the user.
    /// This is relevant only to the UI, but is included in the base library
    /// because the logic is independent of the particular UI framework.
    class ContentsCache {
      public:
        /// The EditTree object is never replaced, so we can keep a reference.
        /// (The input/output features can change (e.g. after a reload) so we
        /// can't store them in this object.)
        ContentsCache(const EditTree& edits);

        /// Build the cache with the given input and output features.
        void setFeatures(const babelwires::Feature* inputFeature, const babelwires::Feature* outputFeature);

        /// Update the part of the cache concerning modifiers.
        void updateModifierCache();

        /// Get the number of visible rows.
        int getNumRows() const;

        /// Get the ith visible row.
        const ContentsCacheEntry* getEntry(int i) const;

        /// Return the index if the path is in one of the visible rows.
        /// Otherwise return -1.
        int getIndexOfPath(bool seekInputFeature, const FeaturePath& path) const;

        /// Describes the way an element may have changed.
        enum class Changes : unsigned int {
            NothingChanged = 0b0,

            StructureChanged = 0b1,

            SomethingChanged = 0b1
        };

        /// Query the feature element for changes.
        bool isChanged(Changes changes) const;

        /// Clear any changes the element is carrying.
        void clearChanges();

      private:
        /// Determine whether the root is visible and set the indexOffset accordingly.
        void setIndexOffset();

        /// Set flags recording a change.
        void setChanged(Changes changes);

        /// Update the flags for modifiers in the entries.
        void updateModifierFlags();

      private:
        /// The rows of the contents. The first row may be hidden if there's no useful information there.
        std::vector<ContentsCacheEntry> m_rows;
        const EditTree& m_edits;

        /// We hide the roots unless necessary (root is a file or there are hidden failed modifiers)
        int m_indexOffset = 0;

        Changes m_changes;
    };

    DEFINE_ENUM_FLAG_OPERATORS(ContentsCache::Changes);

} // namespace babelwires