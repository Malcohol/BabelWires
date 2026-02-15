/**
 * Holds the identifier of a field.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/Detail/identifierValueBase.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

namespace babelwires {

    /// Holds the identifier of a field.
    /// This allows field identifiers to be made available to the RecordTypeConstructor.
    class FieldIdValue : public IdentifierValueBase<ShortId> {
      public:
        CLONEABLE(FieldIdValue);
        SERIALIZABLE(FieldIdValue, "fieldId", EditableValue, 1);

        FieldIdValue();
        FieldIdValue(ShortId value, RecordType::Optionality optionality = RecordType::Optionality::alwaysActive );

        RecordType::Optionality getOptionality() const { return m_optionality; }
        void setOptionality(RecordType::Optionality isOptional) { m_optionality = isOptional; }

        void serializeContents(Serializer& serializer) const override;
        Result deserializeContents(Deserializer& deserializer) override;
        std::string toString() const override;

        std::size_t getHash() const override;
        bool operator==(const Value& other) const override;

      private:
        RecordType::Optionality m_optionality = RecordType::Optionality::alwaysActive;
    };
} // namespace babelwires
