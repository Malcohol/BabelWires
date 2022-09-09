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

        /// Select the tag.
        void selectTag(Identifier tag);

        /// Return the tag which is currently selected.
        Identifier getSelectedTag() const;

        /// Return the index of the tag which is currently selected.
        unsigned int getSelectedTagIndex() const;

        /// Get the fields of the currently selected branch.
        const std::vector<Identifier>& getFieldsOfSelectedBranch() const;

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
                                                  const Identifier& fieldIdentifier) {
        T* fTPtr = f.get();
        addFieldInBranchInternal(tag, FieldAndIndex{fieldIdentifier, std::move(f), getNumFeatures()});
        return fTPtr;
    }
} // namespace babelwires