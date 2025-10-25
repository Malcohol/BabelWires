/**
 * RecordType carries a sequence of Fields (some of which can be inactive).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/compoundType.hpp>

namespace babelwires {

    /// RecordType carries a sequence of Fields (some of which are optional and can be inactive).
    class RecordType : public CompoundType {
      public:
        enum class Optionality { alwaysActive, optionalDefaultInactive, optionalDefaultActive };

        struct Field {
            ShortId m_identifier;
            TypeRef m_type;
            Optionality m_optionality = Optionality::alwaysActive;
        };

        RecordType(std::vector<Field> fields);

        /// A convenience method for defining a record with all the fields of "parent" plus "additionalFields".
        /// This ensures a subtype relationship between this and parent, and ensures that the relationship is
        /// maintained even if the supertype changes.
        /// Note that you don't have to use this method to establish that relationship, since subtyping is
        /// defined by the sets of fields. This is similar to duck-typing, but since field identifiers are globally
        /// unique, a subtyping relationship should never arise unintentionally.
        RecordType(const RecordType& parent, std::vector<Field> additionalFields);

        std::string getFlavour() const override;

        /// Active the field, so it appears in the record.
        void activateField(const TypeSystem& typeSystem, ValueHolder& value, ShortId fieldId) const;

        /// Deactivate the field, so it does not appear in the record.
        /// This operation sets the subfeature to its default state.
        void deactivateField(ValueHolder& value, ShortId fieldId) const;

        /// Ensure the optionals in the value have the state as specified in the arrays.
        void ensureActivated(const TypeSystem& typeSystem, ValueHolder& value,
                             const std::vector<ShortId>& optionalsToEnsureActivated) const;

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
        /// Get a reference to the child's value using its field identifier.
        std::tuple<const ValueHolder&, const TypeRef&> getChildById(const ValueHolder& compoundValue,
                                                                    ShortId fieldId) const;

        /// Get a non-const reference to the child's value using its field identifier.
        std::tuple<ValueHolder&, const TypeRef&> getChildByIdNonConst(ValueHolder& compoundValue,
                                                                      ShortId fieldId) const;

      public:
        NewValueHolder createValue(const TypeSystem& typeSystem) const override;
        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        unsigned int getNumChildren(const ValueHolder& compoundValue) const override;
        std::tuple<const ValueHolder*, PathStep, const TypeRef&> getChild(const ValueHolder& compoundValue,
                                                                          unsigned int i) const override;
        std::tuple<ValueHolder*, PathStep, const TypeRef&> getChildNonConst(ValueHolder& compoundValue,
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
