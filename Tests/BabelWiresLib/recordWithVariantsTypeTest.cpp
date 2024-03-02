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

    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagAId()), 0);
    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagBId()), 1);
    EXPECT_EQ(recordType.getIndexOfTag(testUtils::TestRecordWithVariantsType::getTagCId()), 2);

    EXPECT_TRUE(testUtils::areEqualSets(recordType.getTags(), {testUtils::TestRecordWithVariantsType::getTagAId(),
                                                               testUtils::TestRecordWithVariantsType::getTagBId(),
                                                               testUtils::TestRecordWithVariantsType::getTagCId()}));
}
