/**
 * Holds a single value of integer type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Sum/sumValue.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::SumValue::SumValue() = default;
babelwires::SumValue::SumValue(TypeRef typeTag, EditableValueHolder value)
    : m_typeTag(std::move(typeTag))
    , m_value(std::move(value)) {}

const babelwires::TypeRef& babelwires::SumValue::getTypeTag() const {
    return m_typeTag;
}

const babelwires::EditableValueHolder& babelwires::SumValue::getValue() const {
    return m_value;
}

void babelwires::SumValue::set(TypeRef typeTag, EditableValueHolder value) {}

void babelwires::SumValue::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(m_typeTag);
    serializer.serializeObject(m_value->is<EditableValue>(), "value");
}

void babelwires::SumValue::deserializeContents(Deserializer& deserializer) {
    m_typeTag = std::move(*deserializer.deserializeObject<TypeRef>());
    m_value = std::move(*deserializer.deserializeObject<EditableValue>("value"));
}

void babelwires::SumValue::visitIdentifiers(IdentifierVisitor& visitor) {
    assert(false && "canContainIdentifiers should have returned false");
}

void babelwires::SumValue::visitFilePaths(FilePathVisitor& visitor) {
    assert(false && "canContainFilePaths should have returned false");
}

bool babelwires::SumValue::canContainIdentifiers() const {
    // TypeRefs always contain identifiers.
    return true;
}

bool babelwires::SumValue::canContainFilePaths() const {
    return m_value->canContainFilePaths();
}

std::size_t babelwires::SumValue::getHash() const {
    return hash::mixtureOf(m_typeTag, m_value);
}

bool babelwires::SumValue::operator==(const Value& other) const {
    const SumValue* otherValue = other.as<SumValue>();
    return otherValue && (m_typeTag == otherValue->m_typeTag) && (m_value == otherValue->m_value);
}

std::string babelwires::SumValue::toString() const {
    // It's technically OK to allow ambiguity here and it's likely to be OK for users in practice too.
    return m_value->toString();
}
