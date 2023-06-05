/**
 * Holds a single value of an enum.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

#include <BabelWiresLib/Types/Enum/enumType.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::EnumValue::EnumValue() = default;

babelwires::EnumValue::EnumValue(ShortId value)
    : m_value(value) {}

/// Get the current value of the feature.
babelwires::ShortId babelwires::EnumValue::get() const {
    return m_value;
}

void babelwires::EnumValue::set(ShortId value) {
    m_value = value;
}

void babelwires::EnumValue::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

void babelwires::EnumValue::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("value", m_value);
}

void babelwires::EnumValue::visitIdentifiers(IdentifierVisitor& visitor) {
    visitor(m_value);
}

void babelwires::EnumValue::visitFilePaths(FilePathVisitor& visitor) {}

bool babelwires::EnumValue::canContainIdentifiers() const {
    return true;
}

bool babelwires::EnumValue::canContainFilePaths() const {
    return false;
}

std::size_t babelwires::EnumValue::getHash() const {
    // eeee - Arbitrary discriminator.
    return hash::mixtureOf(0xeeee, m_value);
}

bool babelwires::EnumValue::operator==(const Value& other) const {
    const EnumValue* otherValue = other.as<EnumValue>();
    return otherValue && (m_value == otherValue->m_value);
}

std::string babelwires::EnumValue::toString() const {
    return IdentifierRegistry::read()->getName(m_value);
}
