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

        /// Return the set of tags.
        const TagValues& getTags() const;

      protected:
        void addFieldInBranchInternal(const Identifier& tag, FieldAndIndex fieldAndIndex);
        void doSetToDefault() override;
        void doSetToDefaultNonRecursive() override;

        bool isTag(Identifier tag) const;
        bool isSelectedTag(Identifier tag) const;

      protected:
        /// Those fields which are optional.
        TagValues m_tags;
        unsigned int m_defaultTagIndex;
        std::optional<Identifier> m_selectedTag;

        struct SelectedBranch {
            std::vector<Identifier> m_activeFields;
        };

        struct UnselectedBranch {
            std::vector<FieldAndIndex> m_inactiveFields;
        };

        SelectedBranch m_selectedBranch;
        std::unordered_map<Identifier, UnselectedBranch> m_unselectedBranches;
    };

    template <typename T>
    T* babelwires::UnionFeature::addFieldInBranch(const Identifier& tag, std::unique_ptr<T> f,
                                                  const Identifier& identifier) {
        T* fTPtr = f.get();
        addFieldInBranchInternal(tag, FieldAndIndex{identifier, std::move(f), getNumFeatures()});
        return fTPtr;
    }
} // namespace babelwires
