#include <BabelWiresLib/TypeSystem/enumValue.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

babelwires::EnumValue::EnumValue()
    : m_value("unset") {}

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
    const Enum& e = type.is<Enum>();
    return e.isAValue(m_value);
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
    // TODO
    return 0;
}
