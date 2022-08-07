#include <gtest/gtest.h>

#include <BabelWiresLib/Features/unionFeature.hpp>

#include <BabelWiresLib/Features/numericFeature.hpp>
#include <BabelWiresLib/Features/featureMixins.hpp>

#include <Tests/TestUtils/equalSets.hpp>

TEST(UnionFeatureTest, fieldOrder) {
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature recordFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(recordFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    recordFeature.setToDefault();

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fieldC0);
    EXPECT_EQ(recordFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), fieldC1);

    recordFeature.selectTag(tagB);

    EXPECT_EQ(recordFeature.getNumFeatures(), 2);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature1);

    recordFeature.selectTag(tagA);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), fieldA0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), fieldA1);
    EXPECT_EQ(recordFeature.getFeature(3), fixedFeature1);

    // Same tag again.
    recordFeature.selectTag(tagA);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), fieldA0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), fieldA1);
    EXPECT_EQ(recordFeature.getFeature(3), fixedFeature1);

    recordFeature.selectTag(tagB);

    EXPECT_EQ(recordFeature.getNumFeatures(), 2);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature1);

    recordFeature.selectTag(tagC);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fieldC0);
    EXPECT_EQ(recordFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), fieldC1);

    recordFeature.selectTag(tagA);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), fieldA0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), fieldA1);
    EXPECT_EQ(recordFeature.getFeature(3), fixedFeature1);

    recordFeature.selectTag(tagC);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fieldC0);
    EXPECT_EQ(recordFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), fieldC1);
}

TEST(UnionFeatureTest, changes) {
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature recordFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(recordFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    recordFeature.setToDefault();

    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    recordFeature.clearChanges();
    recordFeature.selectTag(tagA);

    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    recordFeature.clearChanges();
    // Same tag again.
    recordFeature.selectTag(tagA);

    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    recordFeature.clearChanges();
    recordFeature.selectTag(tagB);

    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    recordFeature.clearChanges();
    recordFeature.selectTag(tagC);

    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(UnionFeatureTest, hash) {
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature recordFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(recordFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    recordFeature.setToDefault();

    const size_t hashC = recordFeature.getHash();

    recordFeature.selectTag(tagA);

    const size_t hashA = recordFeature.getHash();
    EXPECT_NE(hashA, hashC);

    recordFeature.selectTag(tagB);

    const size_t hashB = recordFeature.getHash();
    EXPECT_NE(hashB, hashC);
    EXPECT_NE(hashB, hashA);

    recordFeature.selectTag(tagC);

    const size_t hashC_2 = recordFeature.getHash();
    EXPECT_EQ(hashC, hashC_2);

    recordFeature.selectTag(tagB);

    const size_t hashB_2 = recordFeature.getHash();
    EXPECT_EQ(hashB, hashB_2);

    recordFeature.selectTag(tagA);

    const size_t hashA_2 = recordFeature.getHash();
    EXPECT_EQ(hashA, hashA_2);
}

TEST(UnionFeatureTest, queries) {
    babelwires::Identifier tagA("tagA");
    tagA.setDiscriminator(1);
    babelwires::Identifier tagB("tagB");
    tagB.setDiscriminator(1);
    babelwires::Identifier tagC("tagC");
    tagC.setDiscriminator(1);

    babelwires::UnionFeature recordFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 2);

    EXPECT_EQ(recordFeature.getNumFeatures(), 0);

    babelwires::Identifier fieldIdA0("fldA0");
    fieldIdA0.setDiscriminator(1);
    babelwires::IntFeature* fieldA0 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier fieldIdC0("fldC0");
    fieldIdC0.setDiscriminator(1);
    babelwires::IntFeature* fieldC0 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC0);

    babelwires::Identifier fieldIdA1("fldA1");
    fieldIdA1.setDiscriminator(1);
    babelwires::IntFeature* fieldA1 = recordFeature.addFieldInBranch(tagA, std::make_unique<babelwires::IntFeature>(), fieldIdA1);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier fieldIdC1("fldC1");
    fieldIdC1.setDiscriminator(1);
    babelwires::IntFeature* fieldC1 = recordFeature.addFieldInBranch(tagC, std::make_unique<babelwires::IntFeature>(), fieldIdC1);

    // Queries that do not depend on the active state of the union.

    EXPECT_TRUE(testUtils::areEqualSets(recordFeature.getTags(), {tagA, tagB, tagC}));

    EXPECT_TRUE(recordFeature.isTag(tagA));
    EXPECT_TRUE(recordFeature.isTag(tagB));
    EXPECT_TRUE(recordFeature.isTag(tagC));
    EXPECT_FALSE(recordFeature.isTag("flerm"));
    EXPECT_FALSE(recordFeature.isTag(fieldIdA0));
    EXPECT_FALSE(recordFeature.isTag(ff1));

    EXPECT_EQ(recordFeature.getIndexOfTag(tagA), 0);
    EXPECT_EQ(recordFeature.getIndexOfTag(tagB), 1);
    EXPECT_EQ(recordFeature.getIndexOfTag(tagC), 2);

    // Queries that require the union to be in an active state.

    recordFeature.setToDefault();

    EXPECT_EQ(recordFeature.getSelectedTag(), tagC);
    EXPECT_EQ(recordFeature.getSelectedTagIndex(), 2);
    EXPECT_TRUE(testUtils::areEqualSets(recordFeature.getFieldsOfSelectedBranch(), {fieldIdC0, fieldIdC1}));

    recordFeature.selectTag(tagB);

    EXPECT_EQ(recordFeature.getSelectedTag(), tagB);
    EXPECT_EQ(recordFeature.getSelectedTagIndex(), 1);
    EXPECT_TRUE(testUtils::areEqualSets(recordFeature.getFieldsOfSelectedBranch(), {}));

    recordFeature.selectTag(tagA);

    EXPECT_EQ(recordFeature.getSelectedTag(), tagA);
    EXPECT_EQ(recordFeature.getSelectedTagIndex(), 0);
    EXPECT_TRUE(testUtils::areEqualSets(recordFeature.getFieldsOfSelectedBranch(), {fieldIdA0, fieldIdA1}));
}