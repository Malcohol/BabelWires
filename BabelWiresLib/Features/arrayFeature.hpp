/**
 * An ArrayFeature can contain a dynamically-sized sequence of features.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/compoundFeature.hpp>
#include <Common/types.hpp>

namespace babelwires {
    class ValueNames;

    /// An dynamically-sized sequence of features.
    /// Because "element" is used elsewhere, I'm fixed on "entry" for the contents.
    class ArrayFeature : public CompoundFeature {
      public:
        /// Get the valid size range of this array.
        Range<unsigned int> getSizeRange() const;

        /// Add an entry to the array.
        /// If indexOfNewElement is negative, the element is put at the end of the array.
        Feature* addEntry(int indexOfNewElement = -1);

        /// Remove an entry from the array.
        /// If indexOfNewElement is negative, the last element is removed.
        void removeEntry(int indexOfEntryToRemove);

        virtual int getNumFeatures() const override;
        virtual PathStep getStepToChild(const Feature* child) const override;
        virtual Feature* tryGetChildFromStep(const PathStep& step) override;
        virtual const Feature* tryGetChildFromStep(const PathStep& step) const override;

        /// Throws a ModelException if the newSize is outside the size range.
        void setSize(unsigned int newSize);

        /// Make the structure of this array like the structure of the other array.
        /// This is not deep, and the contents of the entries which survive are not
        /// effected.
        void copyStructureFrom(const ArrayFeature& other);

        /// If particular indices have a specific meaning, they can be given names.
        /// The default implementation returns nullptr.
        virtual const ValueNames* getEntryNames() const;

      protected:
        virtual void doSetToDefault() override;
        virtual void doSetToDefaultNonRecursive() override;
        virtual std::size_t doGetHash() const override;
        virtual Feature* doGetFeature(int i) override;
        virtual const Feature* doGetFeature(int i) const override;
        virtual Range<unsigned int> doGetSizeRange() const;

        /// Return null if an element should not be added.
        virtual std::unique_ptr<Feature> createNextEntry() const = 0;

      protected:
        using EntryId = std::uint16_t;

        struct Entry {
            std::unique_ptr<Feature> m_feature;

            /// Uniquely identifies this entry, even after array modifications.
            /// The ID is not preserved by undo / redo, although uniqueness is still guaranteed.
            EntryId m_id = std::numeric_limits<std::uint16_t>::max();
        };
        std::vector<Entry> m_entries;

        /// The ID to assign to the next entry added.
        EntryId m_nextId = 0;
    };

    /// An arbitrarily sized array feature which always creates default constructed entries.
    template <typename ENTRY_FEATURE> class StandardArrayFeature : public ArrayFeature {
      public:
        virtual std::unique_ptr<Feature> createNextEntry() const override { return std::make_unique<ENTRY_FEATURE>(); }
    };

} // namespace babelwires
