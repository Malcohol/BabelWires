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

namespace babelwires {

    /// A Record feature which offers alternative sets of fields.
    /// The record can have normal fields which are always available.
    /// Other fields are associated with one of a set of tags, which controls whether they are active or not.
    /// Not every tag needs to have fields associated with it.
    class UnionFeature : public RecordFeature {
      public:
        using TagValues = std::vector<Identifier>;

        /// Construct a union with the given set of tags.
        /// The tags' identifiers must be registered.
        UnionFeature(TagValues tags, unsigned int defaultTagIndex);

        /// Return the set of tags.
        const TagValues& getTags() const;

        /// Get the index of the given tag.
        unsigned int getIndexOfTag(Identifier tag) const;

        /// Check whether the tag is a tag of this union.
        bool isTag(Identifier tag) const;

        /// Add a field to the branch corresponding to the given tag.
        template <typename T>
        T* addFieldInBranch(const Identifier& tag, std::unique_ptr<T> f, const Identifier& fieldIdentifier);

        /// Add a field to the branches corresponding to the given tags.
        template <typename T>
        T* addFieldInBranches(const std::vector<Identifier>& tags, std::unique_ptr<T> f, const Identifier& fieldIdentifier);

        /// Select the tag.
        void selectTag(Identifier tag);

        /// Return the tag which is currently selected.
        Identifier getSelectedTag() const;

        /// Return the index of the tag which is currently selected.
        unsigned int getSelectedTagIndex() const;

        /// Get the fields which would be removed if the proposedTag was selected.
        std::vector<Identifier> getFieldsRemovedByChangeOfBranch(Identifier proposedTag) const;

      protected:
        void addFieldInBranchesInternal(const std::vector<Identifier>& tags, Field field);
        void doSetToDefault() override;
        void doSetToDefaultNonRecursive() override;

        /// Select the tag using an index.
        void selectTagByIndex(unsigned int index);

        struct BranchAdjustment {
            std::vector<Identifier> m_fieldsToRemove;
            std::vector<Identifier> m_fieldsToAdd;
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

        std::unordered_map<Identifier, FieldInfo> m_fieldInfo;

        /// In tag index order.
        std::vector<std::vector<Identifier>> m_fieldsInBranches;
    };

    template <typename T>
    T* babelwires::UnionFeature::addFieldInBranch(const Identifier& tag, std::unique_ptr<T> f,
                                                  const Identifier& fieldIdentifier) {
        return addFieldInBranches({tag}, std::move(f), fieldIdentifier);
    }

    template <typename T>
    T* babelwires::UnionFeature::addFieldInBranches(const std::vector<Identifier>& tags, std::unique_ptr<T> f,
                                                  const Identifier& fieldIdentifier) {
        T* fTPtr = f.get();
        addFieldInBranchesInternal(tags, Field{fieldIdentifier, std::move(f)});
        return fTPtr;
    }
} // namespace babelwires
