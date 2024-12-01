#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/FeatureElements/node.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(SetExpandedCommandTest, executeAndUndoTrue) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestComplexRecordElementData());

    const babelwires::Node* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, testUtils::TestComplexRecordElementData::getPathToRecordSubrecord(), true);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_FALSE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.undo(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));
}

TEST(SetExpandedCommandTest, executeAndUndoFalse) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;
    elementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord());

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);

    const babelwires::Node* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, testUtils::TestComplexRecordElementData::getPathToRecordSubrecord(), false);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.undo(testEnvironment.m_project);

    EXPECT_TRUE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(element->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));
}

TEST(SetExpandedCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SetExpandedCommand command("Test command", 51,
                                           babelwires::Path::deserializeFromString("qqq/zzz"), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoFeature) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestComplexRecordElementData());

    babelwires::SetExpandedCommand command("Test command", elementId,
                                           babelwires::Path::deserializeFromString("qqq/zzz"), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoCompound) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestComplexRecordElementData());

    babelwires::SetExpandedCommand command("Test command", elementId, testUtils::TestComplexRecordElementData::getPathToRecordInt0(), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
