#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(SetExpandedCommandTest, executeAndUndoTrue) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    babelwires::SetExpandedCommand testCopyConstructor("Test command", elementId, testUtils::TestComplexRecordElementData::getPathToRecordSubrecord(), true);
    babelwires::SetExpandedCommand command = testCopyConstructor;

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_FALSE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.undo(testEnvironment.m_project);

    EXPECT_FALSE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));
}

TEST(SetExpandedCommandTest, executeAndUndoFalse) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;
    elementData.m_expandedPaths.emplace_back(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord());

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, testUtils::TestComplexRecordElementData::getPathToRecordSubrecord(), false);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.undo(testEnvironment.m_project);

    EXPECT_TRUE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(node->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecordSubrecord()));
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

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    babelwires::SetExpandedCommand command("Test command", elementId,
                                           babelwires::Path::deserializeFromString("qqq/zzz"), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoCompound) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    babelwires::SetExpandedCommand command("Test command", elementId, testUtils::TestComplexRecordElementData::getPathToRecordInt0(), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
