#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>

#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

TEST(SetExpandedCommandTest, executeAndUndoTrue) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, testUtils::TestRootFeature::s_pathToArray,
                                           true);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_FALSE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));

    command.undo(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));

    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));
}

TEST(SetExpandedCommandTest, executeAndUndoFalse) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData elementData;
    elementData.m_expandedPaths.emplace_back(testUtils::TestRootFeature::s_pathToArray);

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);

    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, testUtils::TestRootFeature::s_pathToArray,
                                           false);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));

    command.undo(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));

    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(testUtils::TestRootFeature::s_pathToArray));
}

TEST(SetExpandedCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SetExpandedCommand command("Test command", 51,
                                           babelwires::FeaturePath::deserializeFromString("qqq/zzz"), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoFeature) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SetExpandedCommand command("Test command", 51,
                                           babelwires::FeaturePath::deserializeFromString("qqq/zzz"), true);

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoCompound) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SetExpandedCommand command("Test command", 51, testUtils::TestRootFeature::s_pathToInt2, true);

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
