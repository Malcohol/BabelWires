#include <gtest/gtest.h>

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(UnionFeatureTest, changes) {
    babelwires::ShortId tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::ShortId tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::ShortId tagC("tagC");
    tagC.setDiscriminator(1);

    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::UnionFeature> rootFeature(testEnvironment.m_projectContext, babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);
    babelwires::UnionFeature& unionFeature = rootFeature.getFeature();

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

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

    babelwires::ShortId fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::ShortId ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::ShortId fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    unionFeature.setToDefault();

    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    unionFeature.clearChanges();
    unionFeature.selectTag(tagA);

    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(unionFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    unionFeature.clearChanges();
    // Same tag again.
    unionFeature.selectTag(tagA);

    EXPECT_FALSE(unionFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(unionFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(unionFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    unionFeature.clearChanges();
    unionFeature.selectTag(tagB);

    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(unionFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    unionFeature.clearChanges();
    unionFeature.selectTag(tagC);

    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(unionFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(unionFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(UnionFeatureTest, hash) {
    babelwires::ShortId tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::ShortId tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::ShortId tagC("tagC");
    tagC.setDiscriminator(1);

    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::UnionFeature> rootFeature(testEnvironment.m_projectContext, babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);
    babelwires::UnionFeature& unionFeature = rootFeature.getFeature();

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

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

    babelwires::ShortId fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::ShortId ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::ShortId fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    unionFeature.setToDefault();

    const size_t hashC = unionFeature.getHash();

    unionFeature.selectTag(tagA);

    const size_t hashA = unionFeature.getHash();
    EXPECT_NE(hashA, hashC);

    unionFeature.selectTag(tagB);

    const size_t hashB = unionFeature.getHash();
    EXPECT_NE(hashB, hashC);
    EXPECT_NE(hashB, hashA);

    unionFeature.selectTag(tagC);

    const size_t hashC_2 = unionFeature.getHash();
    EXPECT_EQ(hashC, hashC_2);

    unionFeature.selectTag(tagB);

    const size_t hashB_2 = unionFeature.getHash();
    EXPECT_EQ(hashB, hashB_2);

    unionFeature.selectTag(tagA);

    const size_t hashA_2 = unionFeature.getHash();
    EXPECT_EQ(hashA, hashA_2);
}

TEST(UnionFeatureTest, queries) {
    babelwires::ShortId tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::ShortId tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::ShortId tagC("tagC");
    tagC.setDiscriminator(1);

    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::UnionFeature> rootFeature(testEnvironment.m_projectContext, babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);
    babelwires::UnionFeature& unionFeature = rootFeature.getFeature();

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

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

    babelwires::ShortId fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::ShortId ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::ShortId fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    // Queries that do not depend on the active state of the union.

    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getTags(), {tagA, tagB, tagC}));

    EXPECT_TRUE(unionFeature.isTag(tagA));
    EXPECT_TRUE(unionFeature.isTag(tagB));
    EXPECT_TRUE(unionFeature.isTag(tagC));
    EXPECT_FALSE(unionFeature.isTag("flerm"));
    EXPECT_FALSE(unionFeature.isTag(fieldIdA0));
    EXPECT_FALSE(unionFeature.isTag(ff1));

    EXPECT_EQ(unionFeature.getIndexOfTag(tagA), 0);
    EXPECT_EQ(unionFeature.getIndexOfTag(tagB), 1);
    EXPECT_EQ(unionFeature.getIndexOfTag(tagC), 2);

    // Queries that require the union to be in an active state.

    unionFeature.setToDefault();

    EXPECT_EQ(unionFeature.getSelectedTag(), tagC);
    EXPECT_EQ(unionFeature.getSelectedTagIndex(), 2);
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsRemovedByChangeOfBranch(tagA), {fieldIdC0, fieldIdC1}));
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsRemovedByChangeOfBranch(tagB), {fieldIdC0, fieldIdC1}));

    unionFeature.selectTag(tagB);

    EXPECT_EQ(unionFeature.getSelectedTag(), tagB);
    EXPECT_EQ(unionFeature.getSelectedTagIndex(), 1);
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsRemovedByChangeOfBranch(tagA), {}));
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsRemovedByChangeOfBranch(tagC), {}));

    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getSelectedTag(), tagA);
    EXPECT_EQ(unionFeature.getSelectedTagIndex(), 0);
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsRemovedByChangeOfBranch(tagB), {fieldIdA0, fieldIdA1}));
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsRemovedByChangeOfBranch(tagC), {fieldIdA0, fieldIdA1}));
}

TEST(UnionFeatureTest, unselectedEnumCanBeDefaulted) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);

    // Confirm that features in branches are in a fully defaulted state when a branch is selected.
    babelwires::ShortId tagA = testUtils::getTestRegisteredIdentifier("tagA");
    babelwires::ShortId tagB = testUtils::getTestRegisteredIdentifier("tagB");

    babelwires::UnionFeature* unionFeature = rootFeature.addField(
        std::make_unique<babelwires::UnionFeature>(babelwires::UnionFeature::TagValues{tagA, tagB}, 0),
        testUtils::getTestRegisteredIdentifier("union"));

    babelwires::RecordFeature* recordA = unionFeature->addFieldInBranch(
        tagA, std::make_unique<babelwires::RecordFeature>(), testUtils::getTestRegisteredIdentifier("recA"));

    babelwires::EnumFeature* enumA =
        recordA->addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()),
                          testUtils::getTestRegisteredIdentifier("enumA"));

    babelwires::RecordFeature* recordB = unionFeature->addFieldInBranch(
        tagB, std::make_unique<babelwires::RecordFeature>(), testUtils::getTestRegisteredIdentifier("recB"));

    babelwires::EnumFeature* enumB =
        recordB->addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()),
                          testUtils::getTestRegisteredIdentifier("enumB"));

    rootFeature.setToDefault();

    EXPECT_EQ(babelwires::RootFeature::tryGetRootFeatureAt(*enumA), &rootFeature);

    unionFeature->selectTag(tagB);

    EXPECT_EQ(babelwires::RootFeature::tryGetRootFeatureAt(*enumA), &rootFeature);
}