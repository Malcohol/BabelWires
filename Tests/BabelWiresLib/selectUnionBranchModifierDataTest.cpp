#include <gtest/gtest.h>

#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Project/Modifiers/selectUnionBranchModifierData.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordWithVariantsType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(SelectUnionBranchModifierDataTest, apply) {
    babelwires::ShortId tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::ShortId tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::ShortId tagC("tagC");
    tagC.setDiscriminator(1);

    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::UnionFeature> rootFeature(
        testEnvironment.m_projectContext, babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 0);
    babelwires::UnionFeature& unionFeature = rootFeature.getFeature();

    babelwires::SelectUnionBranchModifierData data;
    data.m_tagToSelect = tagC;

    babelwires::ShortId fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::ShortId ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    unionFeature.setToDefault();

    data.apply(&unionFeature);

    EXPECT_EQ(unionFeature.getSelectedTag(), tagC);
}

TEST(SelectUnionBranchModifierDataTest, failureNotATag) {
    babelwires::ShortId tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::ShortId tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::ShortId tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::SelectUnionBranchModifierData data;
    data.m_tagToSelect = "notTag";

    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::UnionFeature> rootFeature(
        testEnvironment.m_projectContext, babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 0);
    babelwires::UnionFeature& unionFeature = rootFeature.getFeature();

    babelwires::ShortId fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);

    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    unionFeature.setToDefault();

    EXPECT_THROW(data.apply(&unionFeature), babelwires::ModelException);
}

TEST(SelectUnionBranchModifierDataTest, failureNotAUnion) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectUnionBranchModifierData data;
    data.m_tagToSelect = "tag";

    testUtils::RootedFeature<babelwires::IntFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::IntFeature& notAUnionFeature = rootFeature.getFeature();

    EXPECT_THROW(data.apply(&notAUnionFeature), babelwires::ModelException);
}

TEST(SelectUnionBranchModifierDataTest, clone) {
    babelwires::SelectUnionBranchModifierData data;
    data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("foo/bar/boo");
    data.m_tagToSelect = "tag";

    auto clonePtr = data.clone();
    ASSERT_NE(clonePtr, nullptr);
    EXPECT_EQ(clonePtr->m_pathToFeature, data.m_pathToFeature);
    EXPECT_EQ(clonePtr->m_tagToSelect, data.m_tagToSelect);
}

TEST(SelectUnionBranchModifierDataTest, serialization) {
    std::string serializedContents;
    {
        babelwires::SelectUnionBranchModifierData data;
        data.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("foo/bar/boo");
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
    auto dataPtr = deserializer.deserializeObject<babelwires::SelectUnionBranchModifierData>();
    deserializer.finalize();

    ASSERT_NE(dataPtr, nullptr);
    EXPECT_EQ(dataPtr->m_pathToFeature, babelwires::FeaturePath::deserializeFromString("foo/bar/boo"));
    EXPECT_EQ(dataPtr->m_tagToSelect, "tag");
}

TEST(SelectUnionBranchModifierDataTest, applyToTypes) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::RootedFeature<babelwires::SimpleValueFeature> rootedFeature(
        testEnvironment.m_projectContext, testUtils::TestRecordWithVariantsType::getThisIdentifier());
    babelwires::ValueFeature& valueFeature = rootedFeature.getFeature();
    valueFeature.setToDefault();
    const auto* type = valueFeature.getType().as<testUtils::TestRecordWithVariantsType>();

    EXPECT_EQ(type->getSelectedTag(valueFeature.getValue()), testUtils::TestRecordWithVariantsType::getTagBId());

    babelwires::SelectUnionBranchModifierData data;
    data.m_tagToSelect = testUtils::TestRecordWithVariantsType::getTagAId();

    data.apply(&valueFeature);

    EXPECT_EQ(type->getSelectedTag(valueFeature.getValue()), testUtils::TestRecordWithVariantsType::getTagAId());

    babelwires::SelectUnionBranchModifierData data2;
    data2.m_tagToSelect = testUtils::TestRecordWithVariantsType::getTagCId();

    data2.apply(&valueFeature);

    EXPECT_EQ(type->getSelectedTag(valueFeature.getValue()), testUtils::TestRecordWithVariantsType::getTagCId());
}
