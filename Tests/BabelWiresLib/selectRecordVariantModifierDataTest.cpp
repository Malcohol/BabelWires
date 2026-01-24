#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testRecordWithVariantsType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(SelectRecordVariantModifierDataTest, apply) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(
        testEnvironment.m_typeSystem, testDomain::TestRecordWithVariantsType::getThisIdentifier());
    valueFeature.setToDefault();
    const auto* type = valueFeature.getType()->tryAs<testDomain::TestRecordWithVariantsType>();

    EXPECT_EQ(type->getSelectedTag(valueFeature.getValue()), testDomain::TestRecordWithVariantsType::getTagBId());

    babelwires::SelectRecordVariantModifierData data;
    data.m_tagToSelect = testDomain::TestRecordWithVariantsType::getTagAId();

    data.apply(&valueFeature);

    EXPECT_EQ(type->getSelectedTag(valueFeature.getValue()), testDomain::TestRecordWithVariantsType::getTagAId());

    babelwires::SelectRecordVariantModifierData data2;
    data2.m_tagToSelect = testDomain::TestRecordWithVariantsType::getTagCId();

    data2.apply(&valueFeature);

    EXPECT_EQ(type->getSelectedTag(valueFeature.getValue()), testDomain::TestRecordWithVariantsType::getTagCId());
}

TEST(SelectRecordVariantModifierDataTest, failureNotATag) {
    babelwires::SelectRecordVariantModifierData data;
    data.m_tagToSelect = "notTag";

    testUtils::TestEnvironment testEnvironment;
    babelwires::ValueTreeRoot valueFeature(
        testEnvironment.m_typeSystem, testDomain::TestRecordWithVariantsType::getThisIdentifier());

    valueFeature.setToDefault();

    EXPECT_THROW(data.apply(&valueFeature), babelwires::ModelException);
}

TEST(SelectRecordVariantModifierDataTest, failureNotAUnion) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectRecordVariantModifierData data;
    data.m_tagToSelect = "tag";

    babelwires::ValueTreeRoot notARecordWithVariants(
        testEnvironment.m_typeSystem, babelwires::DefaultIntType::getThisIdentifier());

    EXPECT_THROW(data.apply(&notARecordWithVariants), babelwires::ModelException);
}

TEST(SelectRecordVariantModifierDataTest, clone) {
    babelwires::SelectRecordVariantModifierData data;
    data.m_targetPath = babelwires::Path::deserializeFromString("foo/bar/boo");
    data.m_tagToSelect = "tag";

    auto clonePtr = data.clone();
    ASSERT_NE(clonePtr, nullptr);
    EXPECT_EQ(clonePtr->m_targetPath, data.m_targetPath);
    EXPECT_EQ(clonePtr->m_tagToSelect, data.m_tagToSelect);
}

TEST(SelectRecordVariantModifierDataTest, serialization) {
    std::string serializedContents;
    {
        babelwires::SelectRecordVariantModifierData data;
        data.m_targetPath = babelwires::Path::deserializeFromString("foo/bar/boo");
        data.m_tagToSelect = "tag";

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(data);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }
    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::SelectRecordVariantModifierData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_targetPath, babelwires::Path::deserializeFromString("foo/bar/boo"));
    EXPECT_EQ(dataPtr->m_tagToSelect, "tag");
}

