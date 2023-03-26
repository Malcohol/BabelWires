#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testRootFeature.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(RootFeatureTest, rootedFeature) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestRootFeature rootedFeature(testEnvironment.m_projectContext);

    const babelwires::RootFeature* rootFeature = babelwires::RootFeature::tryGetRootFeatureAt(*rootedFeature.m_intFeature2);
    EXPECT_NE(rootFeature, nullptr);

    const babelwires::ProjectContext& context = babelwires::RootFeature::getProjectContextAt(*rootedFeature.m_intFeature2);
    EXPECT_EQ(&context, &testEnvironment.m_projectContext);
}

TEST(RootFeatureTest, unrootedFeature) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestRecordFeature unrootedFeature;

    const babelwires::RootFeature* rootFeature = babelwires::RootFeature::tryGetRootFeatureAt(*unrootedFeature.m_intFeature2);
    EXPECT_EQ(rootFeature, nullptr);
}
