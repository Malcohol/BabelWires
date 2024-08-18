/**
 * RecordType carries a sequence of Fields (some of which can be inactive).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

// TODO Remove
//#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace babelwires {

    /// RecordType carries a sequence of Fields (some of which are optional and can be inactive).
    class RecordType : public CompoundType {
      public:
        enum class Optionality { alwaysActive, optionalDefaultInactive /* TODO , optionalDefaultActive*/ };

        struct Field {
            ShortId m_identifier;
            TypeRef m_type;
            Optionality m_optionality = Optionality::alwaysActive;
        };

        RecordType(std::vector<Field> fields);

        std::string getKind() const override;
        
        /// Active the field, so it appears in the record.
        void activateField(const TypeSystem& typeSystem, ValueHolder& value, ShortId fieldId) const;

        /// Deactivate the field, so it does not appear in the record.
        /// This operation sets the subfeature to its default state.
        void deactivateField(ValueHolder& value, ShortId fieldId) const;

        /// Ensure the optionals in the value have the state as specified in the arrays.
        void ensureActivated(const TypeSystem& typeSystem, ValueHolder& value, const std::vector<ShortId>& optionalsToEnsureActivated) const;

        /// Is the given field an optional.
        bool isOptional(ShortId fieldId) const;

        /// Is the given optional field activated?
        bool isActivated(const ValueHolder& value, ShortId fieldId) const;

        /// Get the field information.
        const std::vector<Field>& getFields() const;

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
        SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;
        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
      private:
        const Field& getField(ShortId fieldId) const;
        const Field& getFieldFromChildIndex(const ValueHolder& compoundValue, unsigned int i) const;

      private:
        /// The inactive fields, sorted by activeIndex;
        std::vector<Field> m_fields;

        /// Those fields which are optional.
        std::vector<ShortId> m_optionalFieldIds;
    };

/*
    // TODO Remove
    class TestRecordType : public RecordType {
      public:
        TestRecordType();
        
        PRIMITIVE_TYPE("recordT", "Record", "7ce48078-c2e2-45c8-8c0e-dbad1f9fd771", 1);
    };

    class TestRecordType2 : public RecordType {
      public:
        TestRecordType2();
        
        PRIMITIVE_TYPE("recordT2", "Record2", "199e3fa7-5ddc-46c5-8eab-b66a121dac20", 1);
    };

*/

} // namespace babelwires
