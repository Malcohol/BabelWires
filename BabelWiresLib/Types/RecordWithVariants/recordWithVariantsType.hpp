/**
 * RecordWithVariantsType is like a RecordType but has a number of variants.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace babelwires {

    /// RecordWithVariantsType is like a RecordType but has a number of variants.
    /// Variants are selected using tags. The first tag is the default tag.
    // TODO Allow another tag to be specified as the default.
    // way to implement this without storing the variant tag in the value.
    // Alternatively, finding a way to offer this functionality in the UI while implementing it by
    // a union of record types would be nice.
    // TODO Find an efficient way to unite this with RecordType or implement support for type coercion.
    class RecordWithVariantsType : public CompoundType {
      public:
        using Tags = std::vector<ShortId>;

        struct Field {
            ShortId m_identifier;
            TypeRef m_type;
        };
        struct FieldWithTags : Field {
            /// The tags of the variants containing this field.
            /// Empty means that the field is in every variant.
            Tags m_tags;
        };

        /// The tags vector provides the tags in their preferred order.
        /// The tags used in the fields vector must be found in the tags vector.
        RecordWithVariantsType(Tags tags, std::vector<FieldWithTags> fields, unsigned int defaultTagIndex = 0);

        // For now, this is a separate kind from RecordType.
        std::string getKind() const override;

        /// Return the set of tags.
        const Tags& getTags() const;

        /// Get the index of the given tag.
        unsigned int getIndexOfTag(ShortId tag) const;

        /// Check whether the tag is a tag of this union.
        bool isTag(ShortId tag) const;

        /// Select the tag.
        void selectTag(const TypeSystem& typeSystem, ValueHolder& value, ShortId tag) const;

        /// Return the tag which is currently selected.
        ShortId getSelectedTag(const ValueHolder& value) const;
        
        ShortId getDefaultTag() const;

        std::vector<ShortId> getFieldsRemovedByChangeOfBranch(const ValueHolder& value, ShortId proposedTag) const;
      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue, unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue, unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;
        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;

        bool areDifferentNonRecursively(const ValueHolder& compoundValue, const ValueHolder& otherCompoundValue) const override;
      private:
        struct FieldChanges {
          std::vector<unsigned int> m_fieldsRemoved;
          std::vector<unsigned int> m_fieldsAdded;
        };

        /// How do fields change if the tag of a value is switched from currentTag to proposedTag.
        FieldChanges getFieldChanges(ShortId currentTag, ShortId proposedTag) const;

        /// Get the set of fields that are not associated with any tag.
        std::vector<const Field*> getFixedFields() const;

      private:
        /// The inactive fields, sorted by activeIndex;
        Tags m_tags;

        /// The index of the default tag in the tag array.
        ShortId m_defaultTag;

        // Keep private: see constructor.
        std::vector<FieldWithTags> m_fields;

        /// Caches what fields are available in each variant
        /// Points into the m_fields struct.
        std::unordered_map<ShortId, std::vector<const Field*>> m_tagToVariantCache;
    };

} // namespace babelwires
