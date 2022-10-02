#include <gtest/gtest.h>

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/featureMixins.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Features/unionFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(UnionFeatureTest, fieldOrder) {
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature unionFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    unionFeature.setToDefault();

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldC0);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldC1);

    unionFeature.selectTag(tagB);

    EXPECT_EQ(unionFeature.getNumFeatures(), 2);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature1);

    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fieldA0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(2), fieldA1);
    EXPECT_EQ(unionFeature.getFeature(3), fixedFeature1);

    // Same tag again.
    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fieldA0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(2), fieldA1);
    EXPECT_EQ(unionFeature.getFeature(3), fixedFeature1);

    unionFeature.selectTag(tagB);

    EXPECT_EQ(unionFeature.getNumFeatures(), 2);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature1);

    unionFeature.selectTag(tagC);

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldC0);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldC1);

    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fieldA0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(2), fieldA1);
    EXPECT_EQ(unionFeature.getFeature(3), fixedFeature1);

    unionFeature.selectTag(tagC);

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldC0);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldC1);
}

TEST(UnionFeatureTest, fieldOrderWithOverlappingBranches) {
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature unionFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0 = testUtils::getTestRegisteredIdentifier("fldA0");
    babelwires::Identifier ff0 = testUtils::getTestRegisteredIdentifier("ff0");
    babelwires::Identifier fieldIdAB = testUtils::getTestRegisteredIdentifier("fldAB");
    babelwires::Identifier fieldIdC0 = testUtils::getTestRegisteredIdentifier("fldC0");
    babelwires::Identifier fieldIdA1 = testUtils::getTestRegisteredIdentifier("fldA1");
    babelwires::Identifier ff1 = testUtils::getTestRegisteredIdentifier("ff1");
    babelwires::Identifier fieldIdBC = testUtils::getTestRegisteredIdentifier("fldBC");
    babelwires::Identifier fieldIdC1 = testUtils::getTestRegisteredIdentifier("fldC1");
    babelwires::Identifier fieldIdAC = testUtils::getTestRegisteredIdentifier("fldAC");

    babelwires::IntFeature* fieldA0 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);
    babelwires::IntFeature* fieldAB =
        unionFeature.addFieldInBranches({tagA, tagB}, std::make_unique<babelwires::IntFeature>(), fieldIdAB);
    babelwires::IntFeature* fieldC0 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);
    babelwires::IntFeature* fieldBC =
        unionFeature.addFieldInBranches({tagB, tagC}, std::make_unique<babelwires::IntFeature>(), fieldIdBC);
    babelwires::IntFeature* fieldC1 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);
    babelwires::IntFeature* fieldAC =
        unionFeature.addFieldInBranches({tagA, tagC}, std::make_unique<babelwires::IntFeature>(), fieldIdAC);

    unionFeature.setToDefault();

    EXPECT_EQ(unionFeature.getNumFeatures(), 6);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldC0);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldBC);
    EXPECT_EQ(unionFeature.getFeature(4), fieldC1);
    EXPECT_EQ(unionFeature.getFeature(5), fieldAC);

    unionFeature.selectTag(tagB);

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldAB);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldBC);

    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getNumFeatures(), 6);
    EXPECT_EQ(unionFeature.getFeature(0), fieldA0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(2), fieldAB);
    EXPECT_EQ(unionFeature.getFeature(3), fieldA1);
    EXPECT_EQ(unionFeature.getFeature(4), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(5), fieldAC);

    // Same tag again.
    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getNumFeatures(), 6);
    EXPECT_EQ(unionFeature.getFeature(0), fieldA0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(2), fieldAB);
    EXPECT_EQ(unionFeature.getFeature(3), fieldA1);
    EXPECT_EQ(unionFeature.getFeature(4), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(5), fieldAC);

    unionFeature.selectTag(tagB);

    EXPECT_EQ(unionFeature.getNumFeatures(), 4);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldAB);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldBC);

    unionFeature.selectTag(tagC);

    EXPECT_EQ(unionFeature.getNumFeatures(), 6);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldC0);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldBC);
    EXPECT_EQ(unionFeature.getFeature(4), fieldC1);
    EXPECT_EQ(unionFeature.getFeature(5), fieldAC);

    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getNumFeatures(), 6);
    EXPECT_EQ(unionFeature.getFeature(0), fieldA0);
    EXPECT_EQ(unionFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(2), fieldAB);
    EXPECT_EQ(unionFeature.getFeature(3), fieldA1);
    EXPECT_EQ(unionFeature.getFeature(4), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(5), fieldAC);

    unionFeature.selectTag(tagC);

    EXPECT_EQ(unionFeature.getNumFeatures(), 6);
    EXPECT_EQ(unionFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(unionFeature.getFeature(1), fieldC0);
    EXPECT_EQ(unionFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(unionFeature.getFeature(3), fieldBC);
    EXPECT_EQ(unionFeature.getFeature(4), fieldC1);
    EXPECT_EQ(unionFeature.getFeature(5), fieldAC);
}

TEST(UnionFeatureTest, defaults) {
    // Confirm that features in branches are in a fully defaulted state when a branch is selected.
    babelwires::Identifier tagA = testUtils::getTestRegisteredIdentifier("tagA");
    babelwires::Identifier tagB = testUtils::getTestRegisteredIdentifier("tagB");

    babelwires::UnionFeature unionFeature(babelwires::UnionFeature::TagValues{tagA, tagB}, 0);

    babelwires::RecordFeature* recordA = unionFeature.addFieldInBranch(
        tagA, std::make_unique<babelwires::RecordFeature>(), testUtils::getTestRegisteredIdentifier("recA"));
    babelwires::IntFeature* fieldA =
        recordA->addField(std::make_unique<babelwires::HasStaticDefault<babelwires::IntFeature, 5>>(),
                          testUtils::getTestRegisteredIdentifier("aa"));
    babelwires::RecordFeature* recordF = unionFeature.addFieldInBranch(
        tagA, std::make_unique<babelwires::RecordFeature>(), testUtils::getTestRegisteredIdentifier("recF"));
    babelwires::IntFeature* fieldF =
        recordF->addField(std::make_unique<babelwires::HasStaticDefault<babelwires::IntFeature, 6>>(),
                          testUtils::getTestRegisteredIdentifier("ff"));
    babelwires::RecordFeature* recordB = unionFeature.addFieldInBranch(
        tagB, std::make_unique<babelwires::RecordFeature>(), testUtils::getTestRegisteredIdentifier("recB"));
    babelwires::IntFeature* fieldB =
        recordB->addField(std::make_unique<babelwires::HasStaticDefault<babelwires::IntFeature, 7>>(),
                          testUtils::getTestRegisteredIdentifier("bb"));

    unionFeature.setToDefault();
    EXPECT_EQ(fieldA->get(), 5);
    EXPECT_EQ(fieldF->get(), 6);

    unionFeature.selectTag(tagB);
    EXPECT_EQ(fieldF->get(), 6);
    EXPECT_EQ(fieldB->get(), 7);
}

TEST(UnionFeatureTest, changes) {
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature unionFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
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
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature unionFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
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
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature unionFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(unionFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 =
        unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 =
        unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
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
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);

    // Confirm that features in branches are in a fully defaulted state when a branch is selected.
    babelwires::Identifier tagA = testUtils::getTestRegisteredIdentifier("tagA");
    babelwires::Identifier tagB = testUtils::getTestRegisteredIdentifier("tagB");

    babelwires::UnionFeature* unionFeature = rootFeature.addField(std::make_unique<babelwires::UnionFeature>(babelwires::UnionFeature::TagValues{tagA, tagB}, 0), testUtils::getTestRegisteredIdentifier("union"));

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