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
    constexpr const char c_serializedOptionalValue[] = "optionalDefaultInactive";
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
    if (m_optionality != RecordType::Optionality::alwaysActive) {
        std::string optional = c_serializedOptionalValue;
        serializer.serializeValue(c_serializedOptionality, optional);
    }
}

void babelwires::FieldIdValue::deserializeContents(Deserializer& deserializer) {
    IdentifierValueBase::deserializeContents(deserializer);
    std::string optional;
    if (deserializer.deserializeValue(c_serializedOptionality, optional)) {
        if (optional == c_serializedOptionalValue) {
            m_optionality = RecordType::Optionality::optionalDefaultInactive;
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