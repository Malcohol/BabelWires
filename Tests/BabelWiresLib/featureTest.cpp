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
