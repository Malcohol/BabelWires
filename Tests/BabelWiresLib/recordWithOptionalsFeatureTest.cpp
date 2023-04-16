#include <gtest/gtest.h>

#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>

#include <BabelWiresLib/Features/featureMixins.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(RecordWithOptionalsFeatureTest, fieldOrder) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootFeature.getFeature();

    EXPECT_EQ(recordFeature.getNumFeatures(), 0);

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    babelwires::ShortId op2("op2");
    op2.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature2 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op2);

    babelwires::ShortId ff1("ff1");
    ff1.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature1 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff1);

    babelwires::ShortId op3("op3");
    op3.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature3 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op3);

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
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootFeature.getFeature();

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);
    // TODO: This test won't succeed with a non-default value here, which is indicates a bug.
    optionalFeature0->setToDefault();

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
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootFeature.getFeature();

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::ShortId op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);
    recordFeature.setToDefault();

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
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootFeature.getFeature();

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::ShortId op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

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
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootFeature.getFeature();

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), ff0);

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op0);

    babelwires::ShortId op1("op1");
    op1.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature1 =
        recordFeature.addOptionalField(std::make_unique<babelwires::IntFeature>(), op1);

    EXPECT_THROW(recordFeature.activateField("foo"), babelwires::ModelException);
    EXPECT_THROW(recordFeature.activateField("ff0"), babelwires::ModelException);
    EXPECT_THROW(recordFeature.deactivateField("ff0"), babelwires::ModelException);
    EXPECT_THROW(recordFeature.deactivateField("op0"), babelwires::ModelException);
}

TEST(RecordWithOptionalsFeatureTest, setToDefault) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordWithOptionalsFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordWithOptionalsFeature& recordFeature = rootFeature.getFeature();

    babelwires::ShortId ff0("ff0");
    ff0.setDiscriminator(1);
    babelwires::IntFeature* fixedFeature0 =
        recordFeature.addField(std::make_unique<babelwires::IntFeature>(12), ff0);

    babelwires::ShortId op0("op0");
    op0.setDiscriminator(1);
    babelwires::IntFeature* optionalFeature0 = recordFeature.addOptionalField(
        std::make_unique<babelwires::IntFeature>(7), op0);

    recordFeature.setToDefault();

    EXPECT_EQ(fixedFeature0->get(), 12);

    recordFeature.activateField(op0);
    EXPECT_EQ(optionalFeature0->get(), 7);

    recordFeature.setToDefault();

    EXPECT_FALSE(recordFeature.isActivated(op0));
}

TEST(RecordWithOptionalsFeatureTest, inactiveEnumCanBeDefaulted) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);

    // Confirm that features in branches are in a fully defaulted state when a branch is selected.
    babelwires::ShortId ff0 = testUtils::getTestRegisteredIdentifier("ff0");
    babelwires::ShortId op0 = testUtils::getTestRegisteredIdentifier("op0");

    babelwires::RecordWithOptionalsFeature* recordWithOptionalFeature = rootFeature.addField(
        std::make_unique<babelwires::RecordWithOptionalsFeature>(), testUtils::getTestRegisteredIdentifier("recOpt"));

    babelwires::RecordFeature* fixedFeature0 =
        recordWithOptionalFeature->addField(std::make_unique<babelwires::RecordFeature>(), ff0);
    babelwires::RecordFeature* optionalFeature0 =
        recordWithOptionalFeature->addOptionalField(std::make_unique<babelwires::RecordFeature>(), op0);

    babelwires::EnumFeature* enumA =
        fixedFeature0->addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()),
                                testUtils::getTestRegisteredIdentifier("enumA"));

    babelwires::EnumFeature* enumB =
        optionalFeature0->addField(std::make_unique<babelwires::EnumFeature>(testUtils::TestEnum::getThisIdentifier()),
                                   testUtils::getTestRegisteredIdentifier("enumB"));

    rootFeature.setToDefault();

    EXPECT_EQ(babelwires::RootFeature::tryGetRootFeatureAt(*enumA), &rootFeature);

    recordWithOptionalFeature->activateField(op0);

    EXPECT_EQ(babelwires::RootFeature::tryGetRootFeatureAt(*enumB), &rootFeature);
}