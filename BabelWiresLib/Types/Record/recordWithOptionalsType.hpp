/**
 * RecordWithOptionalsType is like a RecordType but some fields can be inactive.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Record/recordTypeBase.hpp>

// TODO Remove
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    /// RecordWithOptionalsType is like a RecordType but some fields can be inactive.
    class RecordWithOptionalsType : public RecordTypeBase {
      public:
        enum class Optionality { alwaysActive, optionalDefaultInactive, optionalDefaultActive };

        struct FieldWithOptionality : Field {
            Optionality m_optionality = Optionality::alwaysActive;
        };

        RecordWithOptionalsType(std::vector<FieldWithOptionality> fields);

        /// Active the field, so it appears in the record.
        void activateField(const TypeSystem& typeSystem, ValueHolder& value, ShortId fieldId) const;

        /// Deactivate the field, so it does not appear in the record.
        /// This operation sets the subfeature to its default state.
        void deactivateField(ValueHolder& value, ShortId fieldId) const;

        /// Is the given field an optional.
        bool isOptional(ShortId fieldId) const;

        /// Is the given optional field activated?
        bool isActivated(const ValueHolder& value, ShortId fieldId) const;

        /// Get the set of optional fields.
        const std::vector<ShortId>& getOptionalFieldIds() const;

        /// Get the count of the currently active optional fields.
        unsigned int getNumActiveFields(const ValueHolder& value) const;

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue, unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue, unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;

      private:
        const FieldWithOptionality& getField(ShortId fieldId) const;
        const FieldWithOptionality& getFieldFromChildIndex(const ValueHolder& compoundValue, unsigned int i) const;

      private:
        /// The inactive fields, sorted by activeIndex;
        std::vector<FieldWithOptionality> m_fields;

        /// Those fields which are optional.
        std::vector<ShortId> m_optionalFieldIds;
    };

    /*
        class TestRecordType2 : public RecordType {
          public:
            TestRecordType2();

            PRIMITIVE_TYPE("recordT2", "Record2", "199e3fa7-5ddc-46c5-8eab-b66a121dac20", 1);
        };
    */

} // namespace babelwires
