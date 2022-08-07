#include <gtest/gtest.h>

#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>

#include <Tests/TestUtils/equalSets.hpp>

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
    babelwires::IntFeature* fieldA0 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

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
    babelwires::IntFeature* fieldA0 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

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
    babelwires::IntFeature* fieldA0 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

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
    babelwires::IntFeature* fieldA0 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = unionFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = unionFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = unionFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

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
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsOfSelectedBranch(), {fieldIdC0, fieldIdC1}));

    unionFeature.selectTag(tagB);

    EXPECT_EQ(unionFeature.getSelectedTag(), tagB);
    EXPECT_EQ(unionFeature.getSelectedTagIndex(), 1);
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsOfSelectedBranch(), {}));

    unionFeature.selectTag(tagA);

    EXPECT_EQ(unionFeature.getSelectedTag(), tagA);
    EXPECT_EQ(unionFeature.getSelectedTagIndex(), 0);
    EXPECT_TRUE(testUtils::areEqualSets(unionFeature.getFieldsOfSelectedBranch(), {fieldIdA0, fieldIdA1}));
}