#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/setExpandedCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

TEST(SetExpandedCommandTest, executeAndUndoTrue) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const libTestUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, libTestUtils::TestRootFeature::s_pathToArray,
                                           true);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));

    command.undo(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));

    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));
}

TEST(SetExpandedCommandTest, executeAndUndoFalse) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_expandedPaths.emplace_back(libTestUtils::TestRootFeature::s_pathToArray);

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);

    const libTestUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, libTestUtils::TestRootFeature::s_pathToArray,
                                           false);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));

    command.undo(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));

    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRootFeature::s_pathToArray));
}

TEST(SetExpandedCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;
    babelwires::SetExpandedCommand command("Test command", 51,
                                           babelwires::FeaturePath::deserializeFromString("qqq/zzz"), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoFeature) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;
    babelwires::SetExpandedCommand command("Test command", 51,
                                           babelwires::FeaturePath::deserializeFromString("qqq/zzz"), true);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoCompound) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;
    babelwires::SetExpandedCommand command("Test command", 51, libTestUtils::TestRootFeature::s_pathToInt2, true);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
