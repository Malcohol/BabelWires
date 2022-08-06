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

    /// A Record feature with subfeatures which can be inactive.
    class UnionFeature : public RecordFeature {
      public:
        using TagValues = std::vector<Identifier>;

        /// Construct a union with the given set of tags.
        UnionFeature(TagValues tags, unsigned int defaultTagIndex);

        /// Add an optional field, which is not present in the record until activated.
        template <typename T>
        T* addFieldInBranch(const Identifier& tag, std::unique_ptr<T> f, const Identifier& identifier);

        /// Select the tag.
        void selectTag(Identifier tag);

        /// Return the tag which is currently selected.
        Identifier getSelectedTag() const;

        /// Return the set of tags.
        const TagValues& getTags() const;

        /// Return the index of the tag which is currently selected.
        unsigned int getSelectedTagIndex() const;

        /// Get the index of the given tag.
        unsigned int getIndexOfTag(Identifier tag) const;

        /// Check whether the tag is a tag of this union.
        bool isTag(Identifier tag) const;

        /// Get the fields of the currently selected branch.
        const std::vector<Identifier> getFieldsOfSelectedBranch() const;

      protected:
        void addFieldInBranchInternal(const Identifier& tag, FieldAndIndex fieldAndIndex);
        void doSetToDefault() override;
        void doSetToDefaultNonRecursive() override;

        /// Select the tag using an index.
        void selectTagByIndex(unsigned int index);

      protected:
        /// Those fields which are optional.
        TagValues m_tags;
        unsigned int m_defaultTagIndex;
        int m_selectedTagIndex = -1;

        /// Information about the currently selected branch.
        struct SelectedBranch {
            std::vector<Identifier> m_activeFields;
        };

        /// Information about an unselected branch.
        struct UnselectedBranch {
            std::vector<FieldAndIndex> m_inactiveFields;
        };

        /// Information about the currently selected branch.
        SelectedBranch m_selectedBranch;

        /// Information about the unselected branches in tag order.
        /// The selected tag does have an entry, but it is always empty.
        std::vector<UnselectedBranch> m_unselectedBranches;
    };

    template <typename T>
    T* babelwires::UnionFeature::addFieldInBranch(const Identifier& tag, std::unique_ptr<T> f,
                                                  const Identifier& identifier) {
        T* fTPtr = f.get();
        addFieldInBranchInternal(tag, FieldAndIndex{identifier, std::move(f), getNumFeatures()});
        return fTPtr;
    }
} // namespace babelwires
