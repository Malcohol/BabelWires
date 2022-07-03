/**
 * Holds a single value of an enum.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/enumValue.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

babelwires::EnumValue::EnumValue() = default;

babelwires::EnumValue::EnumValue(Identifier value)
    : m_value(value) {}

/// Get the current value of the feature.
babelwires::Identifier babelwires::EnumValue::get() const {
    return m_value;
}

void babelwires::EnumValue::set(Identifier value) {
    m_value = value;
}

bool babelwires::EnumValue::isValid(const Type& type) const {
    if (const Enum* e = type.as<Enum>()) {
        return e->isAValue(m_value);
    }
    return false;
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
