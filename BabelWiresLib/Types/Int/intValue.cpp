/**
 * Holds a single value of integer type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <BabelWiresLib/Types/Int/intType.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

/// Get the current value of the feature.
int babelwires::IntValue::get() const {
    return m_value;
}

void babelwires::IntValue::set(int value) {
    m_value = value;
}

void babelwires::IntValue::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

void babelwires::IntValue::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("value", m_value);
}

void babelwires::IntValue::visitIdentifiers(IdentifierVisitor& visitor) {
    visitor(m_value);
}

void babelwires::IntValue::visitFilePaths(FilePathVisitor& visitor) {}

bool babelwires::IntValue::canContainIdentifiers() const {
    return false;
}

bool babelwires::IntValue::canContainFilePaths() const {
    return false;
}

std::size_t babelwires::IntValue::getHash() const {
    // 1111 - Arbitrary discriminator
    return hash::mixtureOf(0x1111, m_value);
}

bool babelwires::IntValue::operator==(const Value& other) const {
    const IntValue* otherValue = other.as<IntValue>();
    return otherValue && (m_value == otherValue->m_value);
}

std::string babelwires::IntValue::toString() const {
    return std::to_string(m_value);
}
