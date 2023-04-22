/**
 * Holds a single value of integer type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::RationalValue::RationalValue() = default;
babelwires::RationalValue::RationalValue(Rational value) : m_value(value) {}

babelwires::Rational babelwires::RationalValue::get() const {
    return m_value;
}

void babelwires::RationalValue::set(Rational value) {
    m_value = value;
}

void babelwires::RationalValue::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

void babelwires::RationalValue::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("value", m_value);
}

void babelwires::RationalValue::visitIdentifiers(IdentifierVisitor& visitor) {
    assert(false && "canContainIdentifiers should have returned false");
}

void babelwires::RationalValue::visitFilePaths(FilePathVisitor& visitor) {
    assert(false && "canContainFilePaths should have returned false");
}

bool babelwires::RationalValue::canContainIdentifiers() const {
    return false;
}

bool babelwires::RationalValue::canContainFilePaths() const {
    return false;
}

std::size_t babelwires::RationalValue::getHash() const {
    // 1122 - Arbitrary discriminator
    return hash::mixtureOf(0x1122, m_value);
}

bool babelwires::RationalValue::operator==(const Value& other) const {
    const RationalValue* otherValue = other.as<RationalValue>();
    return otherValue && (m_value == otherValue->m_value);
}

std::string babelwires::RationalValue::toString() const {
    return m_value.toString();
}
