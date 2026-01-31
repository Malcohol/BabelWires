/**
 * Holds the identifier of a field.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/fieldIdValue.hpp>

namespace {
    constexpr const char c_serializedOptionality[] = "optionality";
    constexpr const char c_serializedOptionalInactiveValue[] = "optionalDefaultInactive";
    constexpr const char c_serializedOptionalActiveValue[] = "optionalDefaultActive";
}

babelwires::FieldIdValue::FieldIdValue() = default;

babelwires::FieldIdValue::FieldIdValue(ShortId value, RecordType::Optionality optionality)
    : IdentifierValueBase(value)
    , m_optionality(optionality) {}

std::size_t babelwires::FieldIdValue::getHash() const {
    // ffff - Arbitrary discriminator.
    return hash::mixtureOf(0xffff, get(), m_optionality);
}

bool babelwires::FieldIdValue::operator==(const Value& other) const {
    const FieldIdValue *const otherValue = other.tryAs<FieldIdValue>();
    return otherValue && (get() == otherValue->get() && (m_optionality == otherValue->m_optionality));
}

void babelwires::FieldIdValue::serializeContents(Serializer& serializer) const {
    IdentifierValueBase::serializeContents(serializer);
    switch(m_optionality) {
        case RecordType::Optionality::optionalDefaultInactive: {
            const std::string optional = c_serializedOptionalInactiveValue;
            serializer.serializeValue(c_serializedOptionality, optional);
            break;
        }
        case RecordType::Optionality::optionalDefaultActive: {
            const std::string optional = c_serializedOptionalActiveValue;
            serializer.serializeValue(c_serializedOptionality, optional);
            break;
        }
        case RecordType::Optionality::alwaysActive:
            break;
    }
}

void babelwires::FieldIdValue::deserializeContents(Deserializer& deserializer) {
    IdentifierValueBase::deserializeContents(deserializer);
    std::string optional;
    const auto result = deserializer.tryDeserializeValue(c_serializedOptionality, optional);
    THROW_ON_ERROR(result, ParseException);
    if (*result) {
        if (optional == c_serializedOptionalInactiveValue) {
            m_optionality = RecordType::Optionality::optionalDefaultInactive;
        } else if (optional == c_serializedOptionalActiveValue) {
            m_optionality = RecordType::Optionality::optionalDefaultActive;
        } else {
            throw ParseException() << "FieldIdValue has unknown optionality value '" << optional << "'";
        }
    } else {
        m_optionality = RecordType::Optionality::alwaysActive;
    }
}

std::string babelwires::FieldIdValue::toString() const {
    return IdentifierValueBase::toString() + ((m_optionality == RecordType::Optionality::optionalDefaultInactive) ? "?" : "");
}