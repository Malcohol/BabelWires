#include <gtest/gtest.h>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/featureMixins.hpp>
#include <BabelWiresLib/Features/heavyValueFeature.hpp>
#include <BabelWiresLib/Features/numericFeature.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/Features/stringFeature.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/TypeSystem/stringType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(FeatureTest, intFeature) {
    babelwires::IntFeature intFeature;
    EXPECT_EQ(intFeature.getOwner(), nullptr);

    intFeature.setToDefault();
    EXPECT_EQ(intFeature.get(), 0);

    intFeature.set(10);
    EXPECT_EQ(intFeature.get(), 10);

    babelwires::IntFeature intFeature2;
    intFeature2.assign(intFeature);
    EXPECT_EQ(intFeature2.get(), 10);
}

TEST(FeatureTest, intFeatureChanges) {
    babelwires::IntFeature intFeature;

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

TEST(FeatureTest, intFeatureHash) {
    babelwires::IntFeature intFeature;

    intFeature.set(0);
    const std::size_t hashAt0 = intFeature.getHash();

    intFeature.set(10);
    const std::size_t hashAt10 = intFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAt0, hashAt10);
}

TEST(FeatureTest, intFeatureWithRange) {
    babelwires::HasStaticRange<babelwires::IntFeature, 10, 100> intFeature;
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

TEST(FeatureTest, intFeatureWithDefault) {
    babelwires::HasStaticDefault<babelwires::IntFeature, -10> intFeature;

    intFeature.setToDefault();
    EXPECT_EQ(intFeature.get(), -10);
}

TEST(FeatureTest, rationalFeature) {
    babelwires::RationalFeature rationalFeature;

    rationalFeature.setToDefault();
    EXPECT_EQ(rationalFeature.get(), 0);

    rationalFeature.set(babelwires::Rational(23, 54));
    EXPECT_EQ(rationalFeature.get(), babelwires::Rational(23, 54));

    babelwires::RationalFeature rationalFeature2;
    rationalFeature2.assign(rationalFeature);
    EXPECT_EQ(rationalFeature2.get(), babelwires::Rational(23, 54));
}

TEST(FeatureTest, rationalFeatureHash) {
    babelwires::RationalFeature rationalFeature;

    rationalFeature.set(0);
    const std::size_t hashAt0 = rationalFeature.getHash();

    rationalFeature.set(babelwires::Rational(23, 54));
    const std::size_t hashAt23over54 = rationalFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAt0, hashAt23over54);
}

TEST(FeatureTest, stringFeature) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry<babelwires::StringType>();

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(), testUtils::getTestRegisteredIdentifier("aaa"));
    babelwires::StringFeature& stringFeature2 = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(), testUtils::getTestRegisteredIdentifier("bbb"));

    stringFeature.setToDefault();
    EXPECT_EQ(stringFeature.get(), "");

    stringFeature.set("Hello");
    EXPECT_EQ(stringFeature.get(), "Hello");

    stringFeature2.set("Goodbye");
    stringFeature2.assign(stringFeature);
    EXPECT_EQ(stringFeature2.get(), "Hello");
}

TEST(FeatureTest, stringFeatureChanges) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry<babelwires::StringType>();

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(), testUtils::getTestRegisteredIdentifier("aaa"));

    // After construction, everything has changed.
    EXPECT_TRUE(stringFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(stringFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(stringFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    stringFeature.clearChanges();
    EXPECT_FALSE(stringFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(stringFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(stringFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    stringFeature.setToDefault();
    // Don't assume anything about the constructed value, so don't test for value changed.
    EXPECT_FALSE(stringFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    stringFeature.set("Hello");
    EXPECT_TRUE(stringFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(stringFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(stringFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(FeatureTest, stringFeatureHash) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry<babelwires::StringType>();

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(), testUtils::getTestRegisteredIdentifier("aaa"));

    stringFeature.set("");
    const std::size_t hashAtEmpty = stringFeature.getHash();

    stringFeature.set("Hello");
    const std::size_t hashAtHello = stringFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAtEmpty, hashAtHello);
}

TEST(FeatureTest, valueCompatibility) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry<babelwires::StringType>();

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(), testUtils::getTestRegisteredIdentifier("aaa"));
    babelwires::StringFeature& stringFeature2 = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(), testUtils::getTestRegisteredIdentifier("bbb"));

    // Currently, int and rational are incompatible. (We could change that in the future)
    babelwires::IntFeature intFeature, intFeature2;
    babelwires::RationalFeature rationalFeature, rationalFeature2;

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
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_typeSystem.addEntry<babelwires::StringType>();

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
    babelwires::RecordFeature recordFeature;

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
    intFeature->set(0);
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
    babelwires::RecordFeature recordFeature;

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
    babelwires::StandardArrayFeature<babelwires::IntFeature> arrayFeature;

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
    babelwires::StandardArrayFeature<babelwires::IntFeature> arrayFeature;

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
    babelwires::StandardArrayFeature<babelwires::IntFeature> arrayFeature;

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
    LimitedArray arrayFeature;

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

namespace {
    struct TestHeavyValue {
        std::size_t getHash() const { return std::hash<int>()(m_int); }

        bool operator==(const TestHeavyValue& other) const { return m_int == other.m_int; }
        bool operator!=(const TestHeavyValue& other) const { return m_int != other.m_int; }

        int m_int = 0;
    };

    struct TestHeavyFeature : babelwires::HeavyValueFeature<TestHeavyValue> {
        std::string doGetValueType() const override { return "foo"; }
    };
} // namespace

TEST(FeatureTest, heavyValueFeature) {
    TestHeavyFeature heavyValueFeature;
    EXPECT_EQ(heavyValueFeature.getOwner(), nullptr);

    heavyValueFeature.setToDefault();
    EXPECT_EQ(heavyValueFeature.get().m_int, 0);

    TestHeavyValue value0;
    value0.m_int = 15;
    heavyValueFeature.set(value0);
    EXPECT_EQ(heavyValueFeature.get().m_int, 15);

    TestHeavyValue value1;
    value1.m_int = 30;
    heavyValueFeature.set(std::move(value1));
    EXPECT_EQ(heavyValueFeature.get().m_int, 30);

    auto value2 = std::make_unique<TestHeavyValue>();
    value2->m_int = 45;
    heavyValueFeature.set(std::move(value2));
    EXPECT_EQ(heavyValueFeature.get().m_int, 45);
}

TEST(FeatureTest, heavyValueChanges) {
    TestHeavyFeature heavyValueFeature;

    // After construction, everything has changed.
    EXPECT_TRUE(heavyValueFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(heavyValueFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_TRUE(heavyValueFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    heavyValueFeature.clearChanges();

    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    heavyValueFeature.setToDefault();
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    ASSERT_EQ(heavyValueFeature.get().m_int, 0);
    TestHeavyValue value0;
    heavyValueFeature.set(value0);

    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    heavyValueFeature.clearChanges();
    value0.m_int = 15;
    heavyValueFeature.set(value0);
    EXPECT_TRUE(heavyValueFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(heavyValueFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::StructureChanged));

    heavyValueFeature.clearChanges();
    heavyValueFeature.set(value0);
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(heavyValueFeature.isChanged(babelwires::Feature::Changes::StructureChanged));
}

TEST(FeatureTest, heavyValueHash) {
    TestHeavyFeature heavyValueFeature;
    heavyValueFeature.setToDefault();
    std::size_t hashAtDefault = heavyValueFeature.getHash();

    TestHeavyValue value0;
    value0.m_int = 10;
    heavyValueFeature.set(value0);
    std::size_t hashAtTen = heavyValueFeature.getHash();

    EXPECT_NE(hashAtDefault, hashAtTen);
}

TEST(FeatureTest, heavyValueAssign) {
    TestHeavyFeature heavyValueFeature;
    TestHeavyValue value0;
    value0.m_int = 10;
    heavyValueFeature.set(value0);

    TestHeavyFeature heavyValueFeature2;
    heavyValueFeature2.assign(heavyValueFeature);
    EXPECT_EQ(heavyValueFeature2.get().m_int, 10);

    value0.m_int = 20;
    heavyValueFeature.set(value0);
    EXPECT_EQ(heavyValueFeature2.get().m_int, 10);

    heavyValueFeature2.clearChanges();
    heavyValueFeature2.assign(heavyValueFeature);
    EXPECT_EQ(heavyValueFeature2.get().m_int, 20);

    EXPECT_TRUE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_TRUE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::StructureChanged));

    heavyValueFeature2.clearChanges();
    heavyValueFeature2.assign(heavyValueFeature);

    EXPECT_FALSE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::StructureChanged));

    value0.m_int = 30;
    heavyValueFeature.set(value0);
    heavyValueFeature2.set(value0);
    heavyValueFeature2.clearChanges();
    heavyValueFeature2.assign(heavyValueFeature);
    EXPECT_EQ(heavyValueFeature2.get().m_int, 30);
    EXPECT_FALSE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::SomethingChanged));
    EXPECT_FALSE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::ValueChanged));
    EXPECT_FALSE(heavyValueFeature2.isChanged(babelwires::Feature::Changes::StructureChanged));
}
