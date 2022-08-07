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

    babelwires::UnionFeature recordFeature(babelwires::UnionFeature::TagValues{tagA, tagB, tagC}, 1);

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

    EXPECT_EQ(recordFeature.getNumFeatures(), 2);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature1);

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


    recordFeature.selectTag(tagB);

    EXPECT_EQ(recordFeature.getNumFeatures(), 2);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature1);
}
