#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordWithVariantsType.hpp>

#include <Tests/TestUtils/equalSets.hpp>

TEST(RecordWithVariantsTypeTest, tags) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    EXPECT_TRUE(recordType.isTag(testUtils::TestRecordWithVariantsType::getTagAId()));
    EXPECT_TRUE(recordType.isTag(testUtils::TestRecordWithVariantsType::getTagBId()));
    EXPECT_TRUE(recordType.isTag(testUtils::TestRecordWithVariantsType::getTagCId()));

    EXPECT_FALSE(recordType.isTag(testUtils::getTestRegisteredIdentifier("foo")));

    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagAId()), 0);
    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagBId()), 1);
    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagCId()), 2);

    EXPECT_TRUE(testUtils::areEqualSets(recordType.getTags(), {testUtils::TestRecordWithVariantsType::getTagAId(),
                                                               testUtils::TestRecordWithVariantsType::getTagBId(),
                                                               testUtils::TestRecordWithVariantsType::getTagCId()}));
}

TEST(RecordWithVariantsTypeTest, value) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestRecordWithVariantsType recordType;

    babelwires::ValueHolder newValue = recordType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* newRecordValue = newValue->as<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);

    // The second tag of the test record is the default.
    EXPECT_EQ(newRecordValue->getTag(), testUtils::TestRecordWithVariantsType::getTagBId());
    EXPECT_EQ(recordType.getSelectedTag(newValue), testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagAId());

    newRecordValue = newValue->as<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_EQ(newRecordValue->getTag(), testUtils::TestRecordWithVariantsType::getTagAId());
    EXPECT_EQ(recordType.getSelectedTag(newValue), testUtils::TestRecordWithVariantsType::getTagAId());

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);

    recordType.selectTag(testEnvironment.m_typeSystem, newValue, testUtils::TestRecordWithVariantsType::getTagCId());
    
    newRecordValue = newValue->as<babelwires::RecordWithVariantsValue>();
    EXPECT_NE(newRecordValue, nullptr);

    EXPECT_EQ(newRecordValue->getTag(), testUtils::TestRecordWithVariantsType::getTagCId());
    EXPECT_EQ(recordType.getSelectedTag(newValue), testUtils::TestRecordWithVariantsType::getTagCId());

    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf0Id()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFf1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldA1Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldB0Id()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldABId()), nullptr);
    EXPECT_NE(newRecordValue->tryGetValue(testUtils::TestRecordWithVariantsType::getFieldBCId()), nullptr);
    EXPECT_EQ(newRecordValue->tryGetValue(babelwires::ShortId("Foo")), nullptr);
}

