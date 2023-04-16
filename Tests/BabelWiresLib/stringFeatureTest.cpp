#include <gtest/gtest.h>

#include <BabelWiresLib/Types/String/stringFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>


TEST(StringFeatureTest, stringFeature) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(),
                                                                     testUtils::getTestRegisteredIdentifier("aaa"));
    babelwires::StringFeature& stringFeature2 = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(),
                                                                      testUtils::getTestRegisteredIdentifier("bbb"));

    stringFeature.setToDefault();
    EXPECT_EQ(stringFeature.get(), "");

    stringFeature.set("Hello");
    EXPECT_EQ(stringFeature.get(), "Hello");

    stringFeature2.set("Goodbye");
    stringFeature2.assign(stringFeature);
    EXPECT_EQ(stringFeature2.get(), "Hello");
}

TEST(StringFeatureTest, stringFeatureChanges) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(),
                                                                     testUtils::getTestRegisteredIdentifier("aaa"));

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

TEST(StringFeatureTest, stringFeatureHash) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::RootFeature rootFeature(testEnvironment.m_projectContext);
    babelwires::StringFeature& stringFeature = *rootFeature.addField(std::make_unique<babelwires::StringFeature>(),
                                                                     testUtils::getTestRegisteredIdentifier("aaa"));

    stringFeature.set("");
    const std::size_t hashAtEmpty = stringFeature.getHash();

    stringFeature.set("Hello");
    const std::size_t hashAtHello = stringFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAtEmpty, hashAtHello);
}
