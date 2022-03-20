#include <BabelWiresLib/TypeSystem/intValue.hpp>

#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

/// Get the current value of the feature.
int babelwires::IntValue::get() const {
    return m_value;
}

void babelwires::IntValue::set(int value) {
    m_value = value;
}

bool babelwires::IntValue::isValid(const TypeSystem& typeSystem, LongIdentifier type) const {
    return true;
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

std::size_t babelwires::IntValue::getHash() const {
    // TODO
    return 0;
}
