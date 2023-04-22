#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Rational/rationalFeature.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRootedFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(RationalFeatureTest, rationalFeature) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RationalFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RationalFeature& rationalFeature = rootFeature.getFeature();

    rationalFeature.setToDefault();
    EXPECT_EQ(rationalFeature.get(), 0);

    rationalFeature.set(babelwires::Rational(23, 54));
    EXPECT_EQ(rationalFeature.get(), babelwires::Rational(23, 54));

    testUtils::RootedFeature<babelwires::RationalFeature> rootFeature2(testEnvironment.m_projectContext);
    babelwires::RationalFeature& rationalFeature2 = rootFeature2.getFeature();

    rationalFeature2.assign(rationalFeature);
    EXPECT_EQ(rationalFeature2.get(), babelwires::Rational(23, 54));
}

TEST(RationalFeatureTest, rationalFeatureHash) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::RootedFeature<babelwires::RationalFeature> rootFeature(testEnvironment.m_projectContext);
    babelwires::RationalFeature& rationalFeature = rootFeature.getFeature();

    rationalFeature.set(0);
    const std::size_t hashAt0 = rationalFeature.getHash();

    rationalFeature.set(babelwires::Rational(23, 54));
    const std::size_t hashAt23over54 = rationalFeature.getHash();

    // There's a small chance that this test will trigger a false positive. If so, convert the test to be more
    // statistical.
    EXPECT_NE(hashAt0, hashAt23over54);
}
