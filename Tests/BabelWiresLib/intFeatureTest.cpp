#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(IntFeatureTest, intFeature) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::IntFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::IntFeature& intFeature = rootFeature.getFeature();

    EXPECT_EQ(intFeature.getOwner(), &rootFeature.getRoot());

    intFeature.setToDefault();
    EXPECT_EQ(intFeature.get(), 0);

    intFeature.set(10);
    EXPECT_EQ(intFeature.get(), 10);

    testUtils::RootedFeature<babelwires::IntFeature> rootedFeature2(testEnvironment.m_projectContext);
    babelwires::IntFeature& intFeature2 = rootFeature.getFeature();

    intFeature2.assign(intFeature);
    EXPECT_EQ(intFeature2.get(), 10);
}

TEST(IntFeatureTest, intFeatureChanges) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::IntFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::IntFeature& intFeature = rootFeature.getFeature();

    // After construction, everything has changed.
    EXPECT_TRUE(intFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(intFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(intFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    intFeature.clearChanges();
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    intFeature.setToDefault();
    // Don't assume anything about the constructed value, so don't test for value changed.
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    intFeature.set(10);
    EXPECT_TRUE(intFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(intFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    intFeature.clearChanges();
    intFeature.set(10);
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(IntFeatureTest, intFeatureHash) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::IntFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::IntFeature& intFeature = rootFeature.getFeature();

    intFeature.set(0);
    const std::size_t hashAt0 = intFeature.getHash();

    intFeature.set(10);
    const std::size_t hashAt10 = intFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAt0, hashAt10);
}

TEST(IntFeatureTest, intFeatureWithRange) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::IntFeature> rootFeature(testEnvironment.m_projectContext, 10, 100);
    babelwires::IntFeature& intFeature = rootFeature.getFeature();

    intFeature.setToDefault();
    EXPECT_EQ(intFeature.get(), 10);

    intFeature.set(50);
    EXPECT_EQ(intFeature.get(), 50);

    intFeature.set(10);
    EXPECT_EQ(intFeature.get(), 10);

    intFeature.set(100);
    EXPECT_EQ(intFeature.get(), 100);

    intFeature.clearChanges();

    EXPECT_THROW(intFeature.set(0), babelwires::ModelException);
    // Unchanged.
    EXPECT_EQ(intFeature.get(), 100);
    EXPECT_FALSE(intFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));

    intFeature.setToDefault();
    EXPECT_EQ(intFeature.get(), 10);
}

TEST(IntFeatureTest, intFeatureWithDefault) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::IntFeature> rootFeature(testEnvironment.m_projectContext, -10);
    babelwires::IntFeature& intFeature = rootFeature.getFeature();

    intFeature.setToDefault();
    EXPECT_EQ(intFeature.get(), -10);
}
