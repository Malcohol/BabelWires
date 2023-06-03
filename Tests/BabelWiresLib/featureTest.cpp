#include <gtest/gtest.h>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Types/Int/intFeature.hpp>
#include <BabelWiresLib/Types/Rational/rationalFeature.hpp>
#include <BabelWiresLib/Types/String/stringFeature.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(FeatureTest, valueCompatibility) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(),
                                                                     testUtils::getTestRegisteredIdentifier("aaa"));
    babelwires::StringFeature& stringFeature2 = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(),
                                                                      testUtils::getTestRegisteredIdentifier("bbb"));

    // Currently, int and rational are incompatible. (We could change that in the future)
    babelwires::IntFeature& intFeature = *rootFeature.addField(std::make_unique<babelwires::IntFeature>(),
                                                               testUtils::getTestRegisteredIdentifier("ccc"));
    babelwires::IntFeature& intFeature2 = *rootFeature.addField(std::make_unique<babelwires::IntFeature>(),
                                                                testUtils::getTestRegisteredIdentifier("ddd"));
    babelwires::RationalFeature& rationalFeature = *rootFeature.addField(
        std::make_unique<babelwires::RationalFeature>(), testUtils::getTestRegisteredIdentifier("eee"));
    babelwires::RationalFeature& rationalFeature2 = *rootFeature.addField(
        std::make_unique<babelwires::RationalFeature>(), testUtils::getTestRegisteredIdentifier("fff"));
    rootFeature.setToDefault();

    EXPECT_TRUE(intFeature.isCompatible(intFeature2));
    EXPECT_FALSE(intFeature.isCompatible(stringFeature));
    EXPECT_FALSE(intFeature.isCompatible(rationalFeature));

    EXPECT_TRUE(rationalFeature.isCompatible(rationalFeature2));
    EXPECT_FALSE(rationalFeature.isCompatible(intFeature));
    EXPECT_FALSE(rationalFeature.isCompatible(stringFeature));

    EXPECT_TRUE(stringFeature.isCompatible(stringFeature2));
    EXPECT_FALSE(stringFeature.isCompatible(rationalFeature));
    EXPECT_FALSE(stringFeature.isCompatible(intFeature));

    EXPECT_NO_THROW(intFeature.assign(intFeature2));
    EXPECT_THROW(intFeature.assign(stringFeature), babelwires::ModelException);
    EXPECT_THROW(intFeature.assign(rationalFeature), babelwires::ModelException);

    EXPECT_NO_THROW(rationalFeature.assign(rationalFeature2));
    EXPECT_THROW(rationalFeature.assign(intFeature), babelwires::ModelException);
    EXPECT_THROW(rationalFeature.assign(stringFeature), babelwires::ModelException);

    EXPECT_NO_THROW(stringFeature.assign(stringFeature2));
    EXPECT_THROW(stringFeature.assign(rationalFeature), babelwires::ModelException);
    EXPECT_THROW(stringFeature.assign(intFeature), babelwires::ModelException);
}

TEST(FeatureTest, recordFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RecordFeature recordFeature;

    EXPECT_EQ(recordFeature.getNumFeatures(), 0);

    babelwires::ShortId hello("Hello");
    hello.setDiscriminator(17);

    auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
    babelwires::IntFeature* intFeature = intFeaturePtr.get();
    recordFeature.addField(std::move(intFeaturePtr), hello);
    EXPECT_EQ(recordFeature.getFeature(0), intFeature);

    EXPECT_EQ(recordFeature.getNumFeatures(), 1);
    babelwires::ShortId hello0 = recordFeature.getFieldIdentifier(0);
    EXPECT_EQ(hello0, hello);
    babelwires::PathStep helloStep(hello);
    // This will set the descriminator (which is mutable) to match the one in the record.
    EXPECT_EQ(recordFeature.tryGetChildFromStep(helloStep), intFeature);
    EXPECT_EQ(hello.getDiscriminator(), helloStep.getField().getDiscriminator());

    const babelwires::ShortId hello1("Hello");
    EXPECT_EQ(hello1.getDiscriminator(), 0);
    // This will set the descriminator (which is mutable) to match the one in the record.
    EXPECT_EQ(recordFeature.getChildIndexFromStep(hello1), 0);
    EXPECT_EQ(hello, hello1);
    EXPECT_EQ(hello.getDiscriminator(), hello1.getDiscriminator());

    const babelwires::ShortId hello2("Hello");
    hello2.setDiscriminator(86);
    // This time the descriminator is untouched, since it's already set.
    EXPECT_EQ(recordFeature.getChildIndexFromStep(hello2), 0);
    EXPECT_EQ(hello, hello1);
    EXPECT_EQ(hello2.getDiscriminator(), 86);

    const babelwires::PathStep step = recordFeature.getStepToChild(intFeature);
    EXPECT_TRUE(step.isField());
    EXPECT_EQ(step.getField(), hello);
    EXPECT_EQ(step.getField().getDiscriminator(), hello.getDiscriminator());

    babelwires::ShortId goodbye("Goodby");
    goodbye.setDiscriminator(109);

    auto stringFeaturePtr = std::make_unique<babelwires::StringFeature>();
    babelwires::StringFeature* stringFeature = stringFeaturePtr.get();
    recordFeature.addField(std::move(stringFeaturePtr), goodbye);

    EXPECT_EQ(recordFeature.getNumFeatures(), 2);
    babelwires::ShortId goodbye0 = recordFeature.getFieldIdentifier(1);
    EXPECT_EQ(goodbye0, goodbye);
    EXPECT_EQ(recordFeature.tryGetChildFromStep(babelwires::PathStep(goodbye0)), stringFeature);

    const babelwires::PathStep step2 = recordFeature.getStepToChild(stringFeature);
    EXPECT_TRUE(step2.isField());
    EXPECT_EQ(step2.getField(), goodbye);

    const babelwires::ShortId goodbye1("Goodby");
    EXPECT_EQ(recordFeature.getChildIndexFromStep(goodbye1), 1);
}

TEST(FeatureTest, recordFeatureChanges) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordFeature& recordFeature = rootFeature.getFeature();

    // After construction, everything has changed.
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    recordFeature.clearChanges();
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    babelwires::ShortId hello("Hello");
    hello.setDiscriminator(17);
    auto intFeaturePtr = std::make_unique<babelwires::IntFeature>();
    babelwires::IntFeature* intFeature = intFeaturePtr.get();
    intFeature->clearChanges();
    recordFeature.addField(std::move(intFeaturePtr), hello);

    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    intFeature->set(21);
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    recordFeature.clearChanges();
    intFeature->set(6);
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(recordFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(recordFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(FeatureTest, recordFeatureHash) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RecordFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RecordFeature& recordFeature = rootFeature.getFeature();

    recordFeature.setToDefault();

    const std::uint32_t hashWhenEmpty = recordFeature.getHash();

    babelwires::ShortId hello("Hello");
    hello.setDiscriminator(17);
    babelwires::IntFeature* intFeature = recordFeature.addField(std::make_unique<babelwires::IntFeature>(), hello);

    intFeature->set(0);
    const std::uint32_t hashWithHello0 = recordFeature.getHash();

    intFeature->set(144);
    const std::uint32_t hashWithHello144 = recordFeature.getHash();

    intFeature->set(0);
    const std::uint32_t hashWithHello0Again = recordFeature.getHash();

    EXPECT_NE(hashWhenEmpty, hashWithHello0);
    EXPECT_NE(hashWhenEmpty, hashWithHello144);
    EXPECT_NE(hashWithHello0, hashWithHello144);
    EXPECT_EQ(hashWithHello0, hashWithHello0Again);

    // Right now, we assume that records do not change their field set dynamically.
    // If we change that, we should compare two records with different field names.
}

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
