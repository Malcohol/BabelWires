#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

testUtils::TestValue::TestValue(std::string value)
    : m_value(value) {}

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

testUtils::TestType::TestType(unsigned int maximumLength, std::string defaultValue)
    : m_maximumLength(maximumLength)
    , m_defaultValue(defaultValue) {
        addTag(getTestTypeTag());
    }

babelwires::NewValueHolder testUtils::TestType::createValue(const babelwires::TypeSystem& typeSystem) const {
    return babelwires::ValueHolder::makeValue<TestValue>(m_defaultValue);
}

bool testUtils::TestType::isValidValue(const babelwires::TypeSystem& typeSystem, const babelwires::Value& value) const {
    const TestValue* const testValue = value.as<TestValue>();
    if (!testValue) {
        return false;
    }
    return (m_maximumLength == 0) || (testValue->m_value.size() <= m_maximumLength);
}

std::string testUtils::TestType::getKind() const {
    return "Test";
}

babelwires::SubtypeOrder testUtils::TestType::compareSubtypeHelper(const babelwires::TypeSystem& typeSystem, const babelwires::Type& other) const {
    const TestType *const otherTestType = other.as<TestType>();
    if (!otherTestType) {
        return babelwires::SubtypeOrder::IsUnrelated;
    }
    // Exploit overflow
    const unsigned int maximumLengthThis = m_maximumLength - 1;
    const unsigned int maximumLengthOther = otherTestType->m_maximumLength - 1;
    if (maximumLengthOther == maximumLengthThis) {
        return babelwires::SubtypeOrder::IsEquivalent;
    }
    return (maximumLengthThis < maximumLengthOther) ? babelwires::SubtypeOrder::IsSubtype : babelwires::SubtypeOrder::IsSupertype;
}

babelwires::Type::Tag testUtils::TestType::getTestTypeTag() {
    return getTestRegisteredMediumIdentifier("TestTypeTag");
}

std::string testUtils::TestType::valueToString(const babelwires::TypeSystem& typeSystem, const babelwires::ValueHolder& v) const { 
    return v->is<TestValue>().toString();
}
