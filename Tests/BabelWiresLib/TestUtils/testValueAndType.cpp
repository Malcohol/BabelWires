#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>


bool  testUtils::TestValue::isValid(const babelwires::Type& type) const{
    return type.as<TestType>();
}

std::size_t testUtils::TestValue::getHash() const{
    return std::hash<std::string>()(m_value);
}

bool testUtils::TestValue::operator==(const Value& other) const{
    if (const TestValue *const otherTestValue = other.as<TestValue>())
    {
        return otherTestValue->m_value == m_value;
    }
    else {
        return false;
    }
}

std::string testUtils::TestValue::toString() const{
    return m_value;
}

void testUtils::TestValue::visitIdentifiers(babelwires::IdentifierVisitor& visitor){}
void testUtils::TestValue::visitFilePaths(babelwires::FilePathVisitor& visitor){}

void testUtils::TestValue::serializeContents(babelwires::Serializer& serializer) const{
    // TODO
}
void testUtils::TestValue::deserializeContents(babelwires::Deserializer& deserializer){
    // TODO
}

babelwires::LongIdentifier testUtils::TestType::getThisIdentifier() {
    return getTestRegisteredLongIdentifier("TestType");
}

testUtils::TestType::TestType()
    : Type(getThisIdentifier(), 1) {}

std::unique_ptr<babelwires::Value> testUtils::TestType::createValue() const {
    return std::make_unique<TestValue>();
}
