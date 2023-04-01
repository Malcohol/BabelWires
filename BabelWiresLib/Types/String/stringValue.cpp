/**
 * Holds a single string.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

babelwires::StringValue::StringValue() = default;
babelwires::StringValue::StringValue(std::string value) : m_value(std::move(value)) {}

/// Get the current value of the feature.
std::string babelwires::StringValue::get() const {
    return m_value;
}

void babelwires::StringValue::set(std::string value) {
    m_value = std::move(value);
}

void babelwires::StringValue::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

void babelwires::StringValue::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("value", m_value);
}

void babelwires::StringValue::visitIdentifiers(IdentifierVisitor& visitor) {
    visitor(m_value);
}

void babelwires::StringValue::visitFilePaths(FilePathVisitor& visitor) {}

bool babelwires::StringValue::canContainIdentifiers() const {
    return false;
}

bool babelwires::StringValue::canContainFilePaths() const {
    return false;
}

std::size_t babelwires::StringValue::getHash() const {
    return std::hash<std::string>()(m_value);
}

bool babelwires::StringValue::operator==(const Value& other) const {
    const StringValue* otherValue = other.as<StringValue>();
    return otherValue && (m_value == otherValue->m_value);
}

std::string babelwires::StringValue::toString() const {
    return m_value;
}
