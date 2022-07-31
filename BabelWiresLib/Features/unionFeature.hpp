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

        struct ActiveBranch {
            std::vector<Identifier> m_activeFields;
        };

        struct InactiveBranch {
            std::vector<FieldAndIndex> m_inactiveFields;
        };

        ActiveBranch m_activeBranch;
        std::unordered_map<Identifier, InactiveBranch> m_inactiveBranches;
    };

    template <typename T>
    T* babelwires::UnionFeature::addFieldInBranch(const Identifier& tag, std::unique_ptr<T> f,
                                                  const Identifier& identifier) {
        T* fTPtr = f.get();
        addFieldInBranchInternal(tag, FieldAndIndex{identifier, std::move(f), getNumFeatures()});
        return fTPtr;
    }
} // namespace babelwires
