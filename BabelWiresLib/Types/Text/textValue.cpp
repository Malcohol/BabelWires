/**
 * Holds a single text value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <BabelWiresLib/Types/Text/textType.hpp>

#include <BaseLib/Serialization/serializer.hpp>
#include <BaseLib/Serialization/deserializer.hpp>

babelwires::TextValue::TextValue() = default;
babelwires::TextValue::TextValue(Text value) : m_value(std::move(value)) {}

babelwires::Text babelwires::TextValue::get() const {
    return m_value;
}

void babelwires::TextValue::set(Text value) {
    m_value = std::move(value);
}

void babelwires::TextValue::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

babelwires::Result babelwires::TextValue::deserializeContents(Deserializer& deserializer) {
    return deserializer.deserializeValue("value", m_value);
}

void babelwires::TextValue::visitIdentifiers(IdentifierVisitor& visitor) {
    assert(false && "canContainIdentifiers should have returned false");
}

void babelwires::TextValue::visitFilePaths(FilePathVisitor& visitor) {
    assert(false && "canContainFilePaths should have returned false");
}

bool babelwires::TextValue::canContainIdentifiers() const {
    return false;
}

bool babelwires::TextValue::canContainFilePaths() const {
    return false;
}

std::size_t babelwires::TextValue::getHash() const {
    return std::hash<std::string>()(std::string(m_value.toUtf8()));
}

bool babelwires::TextValue::operator==(const Value& other) const {
    const TextValue* otherValue = other.tryAs<TextValue>();
    return otherValue && (m_value == otherValue->m_value);
}

std::string babelwires::TextValue::toString() const {
    return std::string(m_value.toUtf8());
}
