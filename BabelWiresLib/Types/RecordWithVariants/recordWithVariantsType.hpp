/**
 * RecordWithVariantsType is like a RecordType but has a number of variants.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

// TODO Remove
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    /// RecordWithVariantsType is like a RecordType but has a number of variants.
    /// Variants are selected using tags.
    // TODO Find a clean and efficient way to unite this with RecordType.
    class RecordWithVariantsType : public CompoundType {
      public:
        using Tags = std::vector<ShortId>;

        struct Field {
            ShortId m_identifier;
            TypeRef m_type;
        };
        struct FieldWithTags : Field {
            Tags m_tags;
        };

        /// The tags vector provides the tags in their preferred order.
        /// The tags used in the fields vector must be found in the tags vector.
        RecordWithVariantsType(Tags tags, std::vector<FieldWithTags> fields);

        // For now, this is a separate kind from RecordType.
        std::string getKind() const override;

        /// Return the set of tags.
        const Tags& getTags() const;

        /// Get the index of the given tag.
        unsigned int getIndexOfTag(ShortId tag) const;

        /// Check whether the tag is a tag of this union.
        bool isTag(ShortId tag) const;

        /// Select the tag.
        void selectTag(const TypeSystem& typeSystem, ValueHolder& value, ShortId tag);

        /// Return the tag which is currently selected.
        ShortId getSelectedTag(const ValueHolder& value) const;
        
        std::vector<ShortId> getFieldsRemovedByChangeOfBranch(const ValueHolder& value, ShortId proposedTag) const;
      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue, unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue, unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;

      private:
        struct FieldChanges {
          std::vector<unsigned int> m_fieldsRemoved;
          std::vector<unsigned int> m_fieldsAdded;
        };

        /// How do fields change if the tag of a value is switched from currentTag to proposedTag.
        FieldChanges getFieldChanges(ShortId currentTag, ShortId proposedTag) const;

      private:
        /// The inactive fields, sorted by activeIndex;
        Tags m_tags;

        // Keep private: see constructor.
        std::vector<FieldWithTags> m_fields;

        /// Caches what fields are available in each variant
        /// Points into the m_fields struct.
        std::unordered_map<ShortId, std::vector<const Field*>> m_tagToVariantCache;
    };

    /*
        class TestRecordType2 : public RecordType {
          public:
            TestRecordType2();

            PRIMITIVE_TYPE("recordT2", "Record2", "199e3fa7-5ddc-46c5-8eab-b66a121dac20", 1);
        };
    */

} // namespace babelwires
