#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/Record/recordValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(RecordTypeTest, simpleRecordTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestSimpleRecordType recordType;

    EXPECT_FALSE(recordType.isOptional(testUtils::TestSimpleRecordType::getInt0Id()));
    EXPECT_FALSE(recordType.isOptional(testUtils::TestSimpleRecordType::getInt1Id()));
    EXPECT_TRUE(recordType.getOptionalFieldIds().empty());

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newRecordValue = newValue->as<babelwires::RecordValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestSimpleRecordType::getInt0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestSimpleRecordType::getInt1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    EXPECT_NE(newRecordValue->getValue(testUtils::TestSimpleRecordType::getInt0Id())->as<babelwires::IntValue>(),
              nullptr);
    EXPECT_NE(newRecordValue->getValue(testUtils::TestSimpleRecordType::getInt1Id())->as<babelwires::IntValue>(),
              nullptr);

    EXPECT_EQ(recordType.getNumActiveFields(newValue), 0);

    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newRecordValue));
    EXPECT_EQ(recordType.getNumChildren(newValue), 2);

    const auto [child0, step0, typeRef0] = recordType.getChild(newValue, 0);
    const auto [child1, step1, typeRef1] = recordType.getChild(newValue, 1);

    EXPECT_NE((*child0)->as<babelwires::IntValue>(), nullptr);
    EXPECT_NE((*child1)->as<babelwires::IntValue>(), nullptr);
    EXPECT_NE(step0.asField(), nullptr);
    EXPECT_NE(step1.asField(), nullptr);
    EXPECT_EQ(*step0.asField(), testUtils::TestSimpleRecordType::getInt0Id());
    EXPECT_EQ(*step1.asField(), testUtils::TestSimpleRecordType::getInt1Id());

    const babelwires::Type& type0 = typeRef0.resolve(testEnvironment.m_typeSystem);
    const babelwires::Type& type1 = typeRef1.resolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type0.as<babelwires::IntType>(), nullptr);
    EXPECT_NE(type1.as<babelwires::IntType>(), nullptr);

    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step0), 0);
    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step1), 1);

}
