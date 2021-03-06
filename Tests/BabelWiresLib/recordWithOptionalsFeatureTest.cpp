#include <gtest/gtest.h>

#include "BabelWiresLib/Features/recordWithOptionalsFeature.hpp"

#include "BabelWiresLib/Features/numericFeature.hpp"
#include "BabelWiresLib/Features/featureMixins.hpp"

#include "Tests/TestUtils/equalSets.hpp"

TEST(RecordWithOptionalsFeatureTest, fieldOrder) {
    babelwires::RecordWithOptionalsFeature recordFeature;

    EXPECT_EQ(recordFeature.getNumFeatures(), 0);

    babelwires::Identifier op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    babelwires::Identifier op2("op2");
    op2.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature2 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op2);

    babelwires::Identifier ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::Identifier op3("op3");
    op3.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature3 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op3);

    EXPECT_EQ(recordFeature.getNumFeatures(), 2);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature1);

    recordFeature.activateField(op0);

    EXPECT_EQ(recordFeature.getNumFeatures(), 3);
    EXPECT_EQ(recordFeature.getFeature(0), optionalFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), fixedFeature1);


    recordFeature.activateField(op2);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), optionalFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), optionalFeature2);
    EXPECT_EQ(recordFeature.getFeature(3), fixedFeature1);

    recordFeature.activateField(op1);

    EXPECT_EQ(recordFeature.getNumFeatures(), 5);
    EXPECT_EQ(recordFeature.getFeature(0), optionalFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), optionalFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), optionalFeature2);
    EXPECT_EQ(recordFeature.getFeature(4), fixedFeature1);

    recordFeature.activateField(op3);

    EXPECT_EQ(recordFeature.getNumFeatures(), 6);
    EXPECT_EQ(recordFeature.getFeature(0), optionalFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), optionalFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), optionalFeature2);
    EXPECT_EQ(recordFeature.getFeature(4), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(5), optionalFeature3);

    recordFeature.deactivateField(op1);

    EXPECT_EQ(recordFeature.getNumFeatures(), 5);
    EXPECT_EQ(recordFeature.getFeature(0), optionalFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), optionalFeature2);
    EXPECT_EQ(recordFeature.getFeature(3), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(4), optionalFeature3);

    recordFeature.deactivateField(op2);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), optionalFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), optionalFeature3);

    recordFeature.activateField(op1);

    EXPECT_EQ(recordFeature.getNumFeatures(), 5);
    EXPECT_EQ(recordFeature.getFeature(0), optionalFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(2), optionalFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(4), optionalFeature3);

    recordFeature.deactivateField(op0);

    EXPECT_EQ(recordFeature.getNumFeatures(), 4);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), optionalFeature1);
    EXPECT_EQ(recordFeature.getFeature(2), fixedFeature1);
    EXPECT_EQ(recordFeature.getFeature(3), optionalFeature3);

    recordFeature.deactivateField(op3);

    EXPECT_EQ(recordFeature.getNumFeatures(), 3);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), optionalFeature1);
    EXPECT_EQ(recordFeature.getFeature(2), fixedFeature1);

    recordFeature.deactivateField(op1);

    EXPECT_EQ(recordFeature.getNumFeatures(), 2);
    EXPECT_EQ(recordFeature.getFeature(0), fixedFeature0);
    EXPECT_EQ(recordFeature.getFeature(1), fixedFeature1);
}

TEST(RecordWithOptionalsFeatureTest, changes) {
    babelwires::RecordWithOptionalsFeature recordFeature;

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    recordFeature.clearChanges();
    
    recordFeature.activateField(op0);

    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    recordFeature.clearChanges();

    recordFeature.deactivateField(op0);

    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(RecordWithOptionalsFeatureTest, hash) {
    babelwires::RecordWithOptionalsFeature recordFeature;

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::Identifier op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    const size_t hash0 = recordFeature.getHash();

    recordFeature.activateField(op0);

    const size_t hash1 = recordFeature.getHash();

    EXPECT_NE(hash0, hash1);

    recordFeature.deactivateField(op0);
    recordFeature.activateField(op1);

    const size_t hash2 = recordFeature.getHash();

    EXPECT_NE(hash0, hash2);
    EXPECT_NE(hash1, hash2);

    recordFeature.activateField(op0);
    recordFeature.deactivateField(op1);

    const size_t hash3 = recordFeature.getHash();

    EXPECT_NE(hash0, hash3);
    EXPECT_NE(hash2, hash3);
    EXPECT_EQ(hash1, hash3);
}

TEST(RecordWithOptionalsFeatureTest, queries) {
    babelwires::RecordWithOptionalsFeature recordFeature;

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::Identifier op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    EXPECT_TRUE(testUtils::areEqualSets(recordFeature.getOptionalFields(), {op0, op1}));

    EXPECT_FALSE(recordFeature.isOptional(ff0));
    EXPECT_TRUE(recordFeature.isOptional(op0));
    EXPECT_TRUE(recordFeature.isOptional(op1));

    EXPECT_FALSE(recordFeature.isActivated(op0));
    EXPECT_FALSE(recordFeature.isActivated(op1));
    EXPECT_EQ(recordFeature.getNumInactiveFields(), 2);

    recordFeature.activateField(op0);

    EXPECT_TRUE(recordFeature.isActivated(op0));
    EXPECT_FALSE(recordFeature.isActivated(op1));
    EXPECT_EQ(recordFeature.getNumInactiveFields(), 1);

    recordFeature.activateField(op1);

    EXPECT_TRUE(recordFeature.isActivated(op0));
    EXPECT_TRUE(recordFeature.isActivated(op1));
    EXPECT_EQ(recordFeature.getNumInactiveFields(), 0);

    recordFeature.deactivateField(op0);

    EXPECT_FALSE(recordFeature.isActivated(op0));
    EXPECT_TRUE(recordFeature.isActivated(op1));
    EXPECT_EQ(recordFeature.getNumInactiveFields(), 1);
}

TEST(RecordWithOptionalsFeatureTest, exceptions) {
    babelwires::RecordWithOptionalsFeature recordFeature;

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::Identifier op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::Identifier op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 = recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    EXPECT_THROW(recordFeature.activateField("foo"), babelwires::ModelException);
    EXPECT_THROW(recordFeature.activateField("ff0"), babelwires::ModelException);
    EXPECT_THROW(recordFeature.deactivateField("ff0"), babelwires::ModelException);
    EXPECT_THROW(recordFeature.deactivateField("op0"), babelwires::ModelException);
}

TEST(RecordWithOptionalsFeatureTest, setToDefault) {
    babelwires::RecordWithOptionalsFeature recordFeature;

    babelwires::Identifier ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::HasStaticDefault<babelwires::IntFeature, 12>>(), ff0);

    babelwires::Identifier op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 = recordFeature.addOptionalField(std::make_unique<babelwires::HasStaticDefault<babelwires::IntFeature, 7>>(), op0);

    recordFeature.setToDefault();

    EXPECT_EQ(fixedFeature0->get(), 12);

    recordFeature.activateField(op0);
    EXPECT_EQ(optionalFeature0->get(), 7);

    recordFeature.setToDefault();

    EXPECT_FALSE(recordFeature.isActivated(op0));
}
