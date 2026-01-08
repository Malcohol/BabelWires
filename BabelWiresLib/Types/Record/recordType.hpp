/**
 * RecordType carries a sequence of Fields (some of which can be inactive).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

#include <map>

namespace babelwires {

    /// RecordType carries a sequence of Fields (some of which are optional and can be inactive).
    class RecordType : public CompoundType {
      public:
        enum class Optionality { alwaysActive, optionalDefaultInactive, optionalDefaultActive };

        struct FieldDefinition {
            ShortId m_identifier;
            TypeExp m_type;
            Optionality m_optionality = Optionality::alwaysActive;
        };

        RecordType(const TypeSystem& typeSystem, const std::vector<FieldDefinition>& fields);

        /// A convenience method for defining a record with all the fields of "parent" plus "additionalFields".
        /// This ensures a subtype relationship between this and parent, and ensures that the relationship is
        /// maintained even if the supertype changes.
        /// Note that you don't have to use this method to establish that relationship, since subtyping is
        /// defined by the sets of fields. This is similar to duck-typing, but since field identifiers are globally
        /// unique, a subtyping relationship should never arise unintentionally.
        RecordType(const TypeSystem& typeSystem, const RecordType& parent, const std::vector<FieldDefinition>& additionalFields);

        /// The storage type, which has a resolved TypePtr instead of a TypeExp.
        struct Field {
            ShortId m_identifier;
            TypePtr m_type;
            Optionality m_optionality;
        };

        /// Construct a record when you have TypePtrs
        RecordType(std::vector<Field> fields);

        std::string getFlavour() const override;

        /// Active the field, so it appears in the record.
        void activateField(const TypeSystem& typeSystem, ValueHolder& value, ShortId fieldId) const;

        /// Deactivate the field, so it does not appear in the record.
        /// This operation sets the subfeature to its default state.
        void deactivateField(ValueHolder& value, ShortId fieldId) const;

        /// Ensure the optionals in the value have the state as specified in the arrays.
        void selectOptionals(const TypeSystem& typeSystem, ValueHolder& value,
                             const std::map<ShortId, bool>& optionalsState) const;

        /// Is the given field an optional.
        bool isOptional(ShortId fieldId) const;

        /// Is the given optional field activated?
        bool isActivated(const ValueHolder& value, ShortId fieldId) const;

        /// Get the set of optional fields.
        const std::vector<ShortId>& getOptionalFieldIds() const;

        /// Get the count of the currently active optional fields.
        unsigned int getNumActiveFields(const ValueHolder& value) const;

        /// Get the field information.
        const std::vector<Field>& getFields() const;

      public:
        /// Get a reference to the child's value using its field identifier.
        std::tuple<const ValueHolder&, TypeExp> getChildById(const ValueHolder& compoundValue,
                                                                    ShortId fieldId) const;

        /// Get a non-const reference to the child's value using its field identifier.
        std::tuple<ValueHolder&, TypeExp> getChildByIdNonConst(ValueHolder& compoundValue,
                                                                      ShortId fieldId) const;

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypePtr&> getChild(const ValueHolder& compoundValue,
                                                                          unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypePtr&> getChildNonConst(ValueHolder& compoundValue,
                            unsigned int i) const override;
        int getChildIndexFromStep(const ValueHolder& compoundValue, const PathStep& step) const override;
        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem,
                                                         const Type& other) const override;
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
} // namespace babelwires
