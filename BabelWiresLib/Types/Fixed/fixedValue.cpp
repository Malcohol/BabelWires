/**
 * Holds a single value of fixed type.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

babelwires::FixedValue::FixedValue() = default;

babelwires::FixedValue::FixedValue(Fixed value)
    : m_value(value) {}

const babelwires::Fixed& babelwires::FixedValue::get() const {
    return m_value;
}

void babelwires::FixedValue::set(Fixed value) {
    m_value = value;
}

void babelwires::FixedValue::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

babelwires::Result babelwires::FixedValue::deserializeContents(Deserializer& deserializer) {
    return deserializer.deserializeValue("value", m_value);
}

void babelwires::FixedValue::visitIdentifiers(IdentifierVisitor& visitor) {
    assert(false && "canContainIdentifiers should have returned false");
}

void babelwires::FixedValue::visitFilePaths(FilePathVisitor& visitor) {
    assert(false && "canContainFilePaths should have returned false");
}

bool babelwires::FixedValue::canContainIdentifiers() const {
    return false;
}

bool babelwires::FixedValue::canContainFilePaths() const {
    return false;
}

std::size_t babelwires::FixedValue::getHash() const {
    return m_value.getHash();
}

bool babelwires::FixedValue::operator==(const Value& other) const {
    const auto* otherValue = other.tryAs<FixedValue>();
    return otherValue && (m_value == otherValue->m_value);
}

std::string babelwires::FixedValue::toString() const {
    return m_value.toString();
}
