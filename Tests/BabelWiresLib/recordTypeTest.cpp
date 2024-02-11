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

#include <Tests/TestUtils/equalSets.hpp>

TEST(RecordTypeTest, simpleRecordTypeValue) {
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
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue()));

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

TEST(RecordTypeTest, complexRecordTypeValue) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestComplexRecordType recordType;

    EXPECT_FALSE(recordType.isOptional(testUtils::TestComplexRecordType::getInt0Id()));
    EXPECT_TRUE(recordType.isOptional(testUtils::TestComplexRecordType::getOpIntId()));
    EXPECT_FALSE(recordType.isOptional(testUtils::TestComplexRecordType::getSubrecordId()));
    EXPECT_FALSE(recordType.isOptional(testUtils::TestComplexRecordType::getInt1Id()));
    EXPECT_TRUE(recordType.isOptional(testUtils::TestComplexRecordType::getOpRecId()));

    EXPECT_TRUE(testUtils::areEqualSets(recordType.getOptionalFieldIds(), {testUtils::TestComplexRecordType::getOpIntId(), testUtils::TestComplexRecordType::getOpRecId()}));
    
    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newRecordValue = newValue->as<babelwires::RecordValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestComplexRecordType::getInt0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestComplexRecordType::getSubrecordId()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestComplexRecordType::getInt1Id()), nullptr);

    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestComplexRecordType::getOpIntId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestComplexRecordType::getOpRecId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    EXPECT_NE(newRecordValue->getValue(testUtils::TestComplexRecordType::getInt0Id())->as<babelwires::IntValue>(),
              nullptr);
    EXPECT_NE(newRecordValue->getValue(testUtils::TestComplexRecordType::getSubrecordId())->as<babelwires::RecordValue>(),
              nullptr);
    EXPECT_NE(newRecordValue->getValue(testUtils::TestComplexRecordType::getInt1Id())->as<babelwires::IntValue>(),
              nullptr);

    EXPECT_EQ(recordType.getNumActiveFields(newValue), 0);

    EXPECT_TRUE(recordType.isValidValue(testEnvironment.m_typeSystem, *newRecordValue));
    EXPECT_FALSE(recordType.isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue()));

    EXPECT_EQ(recordType.getNumChildren(newValue), 3);

    const auto [child0, step0, typeRef0] = recordType.getChild(newValue, 0);
    const auto [child1, step1, typeRef1] = recordType.getChild(newValue, 1);
    const auto [child2, step2, typeRef2] = recordType.getChild(newValue, 2);

    EXPECT_NE((*child0)->as<babelwires::IntValue>(), nullptr);
    EXPECT_NE((*child1)->as<babelwires::RecordValue>(), nullptr);
    EXPECT_NE((*child2)->as<babelwires::IntValue>(), nullptr);
    EXPECT_NE(step0.asField(), nullptr);
    EXPECT_NE(step1.asField(), nullptr);
    EXPECT_NE(step2.asField(), nullptr);
    EXPECT_EQ(*step0.asField(), testUtils::TestComplexRecordType::getInt0Id());
    EXPECT_EQ(*step1.asField(), testUtils::TestComplexRecordType::getSubrecordId());
    EXPECT_EQ(*step2.asField(), testUtils::TestComplexRecordType::getInt1Id());

    const babelwires::Type& type0 = typeRef0.resolve(testEnvironment.m_typeSystem);
    const babelwires::Type& type1 = typeRef1.resolve(testEnvironment.m_typeSystem);
    const babelwires::Type& type2 = typeRef2.resolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type0.as<babelwires::IntType>(), nullptr);
    EXPECT_NE(type1.as<testUtils::TestSimpleRecordType>(), nullptr);
    EXPECT_NE(type2.as<babelwires::IntType>(), nullptr);

    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step0), 0);
    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step1), 1);
    EXPECT_EQ(recordType.getChildIndexFromStep(newValue, step2), 2);
}