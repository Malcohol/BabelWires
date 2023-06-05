/**
 * A UnionFeature is a Record feature with alternative sets of features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/recordFeature.hpp>

#include <optional>
#include <unordered_map>

namespace babelwires {

    /// A Record feature which offers alternative sets of fields.
    /// The record can have normal fields which are always available.
    /// Other fields are associated with one of a set of tags, which controls whether they are active or not.
    /// Not every tag needs to have fields associated with it.
    class UnionFeature : public RecordFeature {
      public:
        using TagValues = std::vector<ShortId>;

        /// Construct a union with the given set of tags.
        /// The tags' identifiers must be registered.
        UnionFeature(TagValues tags, unsigned int defaultTagIndex);

        /// Return the set of tags.
        const TagValues& getTags() const;

        /// Get the index of the given tag.
        unsigned int getIndexOfTag(ShortId tag) const;

        /// Check whether the tag is a tag of this union.
        bool isTag(ShortId tag) const;

        /// Add a field to the branch corresponding to the given tag.
        template <typename T>
        T* addFieldInBranch(const ShortId& tag, std::unique_ptr<T> f, const ShortId& fieldIdentifier);

        /// Add a field to the branches corresponding to the given tags.
        template <typename T>
        T* addFieldInBranches(const std::vector<ShortId>& tags, std::unique_ptr<T> f, const ShortId& fieldIdentifier);

        /// Select the tag.
        void selectTag(ShortId tag);

        /// Return the tag which is currently selected.
        ShortId getSelectedTag() const;

        /// Return the index of the tag which is currently selected.
        unsigned int getSelectedTagIndex() const;

        /// Get the fields which would be removed if the proposedTag was selected.
        std::vector<ShortId> getFieldsRemovedByChangeOfBranch(ShortId proposedTag) const;

      protected:
        void addFieldInBranchesInternal(const std::vector<ShortId>& tags, Field field);
        void doSetToDefault() override;
        void doSetToDefaultNonRecursive() override;

        /// Select the tag using an index.
        void selectTagByIndex(unsigned int index);

        struct BranchAdjustment {
            std::vector<ShortId> m_fieldsToRemove;
            std::vector<ShortId> m_fieldsToAdd;
        };

        BranchAdjustment getBranchAdjustment(unsigned int tagIndex) const;

      protected:
        /// Those fields which are optional.
        TagValues m_tags;
        unsigned int m_defaultTagIndex;
        int m_selectedTagIndex = -1;

        struct TagIndexAndIntendedFieldIndex {
          unsigned int m_tagIndex;
          int m_fieldIndex = -1;
        };

        struct FieldInfo {
          /// The feature will be null when the field is currently active (since it will managed by the record class).
          std::unique_ptr<Feature> m_feature;
          std::vector<TagIndexAndIntendedFieldIndex> m_tagsWithIntendedIndices;
        };

        std::unordered_map<ShortId, FieldInfo> m_fieldInfo;

        /// In tag index order.
        std::vector<std::vector<ShortId>> m_fieldsInBranches;
    };

    template <typename T>
    T* babelwires::UnionFeature::addFieldInBranch(const ShortId& tag, std::unique_ptr<T> f,
                                                  const ShortId& fieldIdentifier) {
        return addFieldInBranches({tag}, std::move(f), fieldIdentifier);
    }

    template <typename T>
    T* babelwires::UnionFeature::addFieldInBranches(const std::vector<ShortId>& tags, std::unique_ptr<T> f,
                                                  const ShortId& fieldIdentifier) {
        T* fTPtr = f.get();
        addFieldInBranchesInternal(tags, Field{fieldIdentifier, std::move(f)});
        return fTPtr;
    }
} // namespace babelwires
