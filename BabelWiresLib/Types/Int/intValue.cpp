/**
 * Holds a single value of integer type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::IntValue::IntValue() : m_value(0) {}
babelwires::IntValue::IntValue(NativeType value) : m_value(value) {}

babelwires::IntValue::NativeType babelwires::IntValue::get() const {
    return m_value;
}

void babelwires::IntValue::set(NativeType value) {
    m_value = value;
}

void babelwires::IntValue::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

void babelwires::IntValue::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("value", m_value);
}

void babelwires::IntValue::visitIdentifiers(IdentifierVisitor& visitor) {
    assert(false && "canContainIdentifiers should have returned false");
}

void babelwires::IntValue::visitFilePaths(FilePathVisitor& visitor) {
    assert(false && "canContainFilePaths should have returned false");
}

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
