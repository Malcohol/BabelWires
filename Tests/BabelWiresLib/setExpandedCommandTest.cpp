#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/setExpandedCommand.hpp>

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(SetExpandedCommandTest, executeAndUndoTrue) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testDomain::TestComplexRecordElementData());

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    babelwires::SetExpandedCommand testCopyConstructor("Test command", elementId, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(), true);
    babelwires::SetExpandedCommand command = testCopyConstructor;

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_FALSE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.undo(testEnvironment.m_project);

    EXPECT_FALSE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.execute(testEnvironment.m_project);

    EXPECT_TRUE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));
}

TEST(SetExpandedCommandTest, executeAndUndoFalse) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestComplexRecordElementData elementData;
    elementData.m_expandedPaths.emplace_back(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(), false);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.undo(testEnvironment.m_project);

    EXPECT_TRUE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(node->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));
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
        testEnvironment.m_project.addNode(testDomain::TestComplexRecordElementData());

    babelwires::SetExpandedCommand command("Test command", elementId,
                                           babelwires::Path::deserializeFromString("qqq/zzz"), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoCompound) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testDomain::TestComplexRecordElementData());

    babelwires::SetExpandedCommand command("Test command", elementId, testDomain::TestComplexRecordElementData::getPathToRecordInt0(), true);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
