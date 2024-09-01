#include <gtest/gtest.h>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(FeatureTest, arrayFeature) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::StandardArrayFeature<babelwires::IntFeature>> rootFeature(
        testEnvironment.m_projectContext);
    babelwires::ArrayFeature& arrayFeature = rootFeature.getFeature();

    EXPECT_EQ(arrayFeature.getNumFeatures(), 0);

    babelwires::Feature* entry = arrayFeature.addEntry();
    EXPECT_NE(entry, nullptr);
    babelwires::IntFeature* firstEntry = entry->as<babelwires::IntFeature>();
    EXPECT_NE(firstEntry, nullptr);

    EXPECT_EQ(arrayFeature.getNumFeatures(), 1);

    EXPECT_EQ(arrayFeature.getFeature(0), firstEntry);

    EXPECT_EQ(arrayFeature.tryGetChildFromStep(0), firstEntry);
    EXPECT_EQ(arrayFeature.tryGetChildFromStep(1), nullptr);

    EXPECT_EQ(arrayFeature.getStepToChild(firstEntry), 0);
    EXPECT_EQ(&arrayFeature.getChildFromStep(0), firstEntry);

    babelwires::IntFeature* secondEntry = static_cast<babelwires::IntFeature*>(arrayFeature.addEntry());

    EXPECT_EQ(arrayFeature.getNumFeatures(), 2);
    EXPECT_EQ(arrayFeature.getFeature(1), secondEntry);
    EXPECT_EQ(arrayFeature.getStepToChild(secondEntry), 1);

    babelwires::IntFeature* newSecondEntry = static_cast<babelwires::IntFeature*>(arrayFeature.addEntry(1));
    babelwires::IntFeature* newFirstEntry = static_cast<babelwires::IntFeature*>(arrayFeature.addEntry(0));
    babelwires::IntFeature* newFourthEntry = static_cast<babelwires::IntFeature*>(arrayFeature.addEntry(4));
    babelwires::IntFeature* newLastEntry = static_cast<babelwires::IntFeature*>(arrayFeature.addEntry());
    EXPECT_EQ(arrayFeature.getNumFeatures(), 6);
    EXPECT_EQ(arrayFeature.getFeature(0), newFirstEntry);
    EXPECT_EQ(arrayFeature.getFeature(1), firstEntry);
    EXPECT_EQ(arrayFeature.getFeature(2), newSecondEntry);
    EXPECT_EQ(arrayFeature.getFeature(3), secondEntry);
    EXPECT_EQ(arrayFeature.getFeature(4), newFourthEntry);
    EXPECT_EQ(arrayFeature.getFeature(5), newLastEntry);

    arrayFeature.removeEntry(0);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 5);
    EXPECT_EQ(arrayFeature.getFeature(0), firstEntry);
    EXPECT_EQ(arrayFeature.getFeature(1), newSecondEntry);
    EXPECT_EQ(arrayFeature.getFeature(2), secondEntry);
    EXPECT_EQ(arrayFeature.getFeature(3), newFourthEntry);
    EXPECT_EQ(arrayFeature.getFeature(4), newLastEntry);

    arrayFeature.removeEntry(3);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 4);
    EXPECT_EQ(arrayFeature.getFeature(0), firstEntry);
    EXPECT_EQ(arrayFeature.getFeature(1), newSecondEntry);
    EXPECT_EQ(arrayFeature.getFeature(2), secondEntry);
    EXPECT_EQ(arrayFeature.getFeature(3), newLastEntry);

    arrayFeature.removeEntry(3);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 3);
    EXPECT_EQ(arrayFeature.getFeature(0), firstEntry);
    EXPECT_EQ(arrayFeature.getFeature(1), newSecondEntry);
    EXPECT_EQ(arrayFeature.getFeature(2), secondEntry);

    arrayFeature.removeEntry(0);
    arrayFeature.removeEntry(1);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 1);
    EXPECT_EQ(arrayFeature.getFeature(0), newSecondEntry);

    arrayFeature.removeEntry(0);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 0);

    arrayFeature.setSize(3);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 3);
}

TEST(FeatureTest, arrayFeatureChanges) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::StandardArrayFeature<babelwires::IntFeature>> rootFeature(
        testEnvironment.m_projectContext);
    babelwires::ArrayFeature& arrayFeature = rootFeature.getFeature();

    // After construction, everything has changed.
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    arrayFeature.clearChanges();
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    arrayFeature.addEntry();

    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    arrayFeature.clearChanges();
    static_cast<babelwires::IntFeature*>(arrayFeature.getFeature(0))->set(100);
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    arrayFeature.clearChanges();
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    arrayFeature.removeEntry(0);
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    arrayFeature.clearChanges();
}

TEST(FeatureTest, arrayFeatureHash) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::StandardArrayFeature<babelwires::IntFeature>> rootFeature(
        testEnvironment.m_projectContext);
    babelwires::ArrayFeature& arrayFeature = rootFeature.getFeature();

    arrayFeature.setToDefault();

    const std::uint32_t hashWhenEmpty = arrayFeature.getHash();

    babelwires::IntFeature* intFeature = static_cast<babelwires::IntFeature*>(arrayFeature.addEntry());

    intFeature->set(0);
    const std::uint32_t hashWithOneEntry0 = arrayFeature.getHash();

    intFeature->set(99);
    const std::uint32_t hashWithOneEntry99 = arrayFeature.getHash();

    intFeature->set(0);
    const std::uint32_t hashWithOneEntry0Again = arrayFeature.getHash();

    arrayFeature.removeEntry(0);
    const std::uint32_t hashWithEmptyAgain = arrayFeature.getHash();

    EXPECT_NE(hashWhenEmpty, hashWithOneEntry0);
    EXPECT_NE(hashWithOneEntry0, hashWithOneEntry99);
    EXPECT_EQ(hashWithOneEntry0, hashWithOneEntry0Again);
    EXPECT_EQ(hashWhenEmpty, hashWithEmptyAgain);
}

namespace {
    struct LimitedArray : babelwires::StandardArrayFeature<babelwires::IntFeature> {
        babelwires::Range<unsigned int> doGetSizeRange() const override {
            return babelwires::Range<unsigned int>{2, 5};
        }
    };
} // namespace

TEST(FeatureTest, arraySizeRange) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<LimitedArray> rootFeature(testEnvironment.m_projectContext);
    babelwires::ArrayFeature& arrayFeature = rootFeature.getFeature();

    arrayFeature.setToDefault();
    EXPECT_EQ(arrayFeature.getNumFeatures(), 2);

    EXPECT_THROW(arrayFeature.removeEntry(0), babelwires::ModelException);
    arrayFeature.addEntry();
    arrayFeature.addEntry();
    arrayFeature.addEntry();
    EXPECT_THROW(arrayFeature.addEntry(), babelwires::ModelException);

    arrayFeature.clearChanges();
    arrayFeature.setSize(3);
    EXPECT_EQ(arrayFeature.getNumFeatures(), 3);
    EXPECT_TRUE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
    arrayFeature.clearChanges();
    EXPECT_THROW(arrayFeature.setSize(0), babelwires::ModelException);
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
    EXPECT_THROW(arrayFeature.setSize(6), babelwires::ModelException);
    EXPECT_FALSE(arrayFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
    EXPECT_EQ(arrayFeature.getNumFeatures(), 3);
}
