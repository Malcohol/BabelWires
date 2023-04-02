#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

testUtils::TestValue::TestValue()
    : m_value("test value value") {}

std::size_t testUtils::TestValue::getHash() const {
    return std::hash<std::string>()(m_value);
}

bool testUtils::TestValue::operator==(const Value& other) const {
    if (const TestValue* const otherTestValue = other.as<TestValue>()) {
        return otherTestValue->m_value == m_value;
    } else {
        return false;
    }
}

std::string testUtils::TestValue::toString() const {
    return m_value;
}

void testUtils::TestValue::visitIdentifiers(babelwires::IdentifierVisitor& visitor) {}
void testUtils::TestValue::visitFilePaths(babelwires::FilePathVisitor& visitor) {}

bool testUtils::TestValue::canContainIdentifiers() const {
    return false;
}

bool testUtils::TestValue::canContainFilePaths() const {
    return false;
}

void testUtils::TestValue::serializeContents(babelwires::Serializer& serializer) const {
    serializer.serializeValue("value", m_value);
}

void testUtils::TestValue::deserializeContents(babelwires::Deserializer& deserializer) {
    deserializer.deserializeValue("value", m_value);
}

babelwires::NewValueHolder testUtils::TestType::createValue() const {
    return babelwires::ValueHolder::makeValue<TestValue>();
}

bool testUtils::TestType::isValidValue(const babelwires::Value& value) const {
    return value.as<TestValue>();
}