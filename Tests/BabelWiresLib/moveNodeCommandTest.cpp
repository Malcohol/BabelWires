#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testNode.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(MoveNodeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestNodeData elementData;
    elementData.m_uiData.m_uiPosition = babelwires::UiPosition{-14, -15};

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const testUtils::TestNode* node =
        testEnvironment.m_project.getNode(elementId)->tryAs<testUtils::TestNode>();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getUiPosition().m_x, -14);
    EXPECT_EQ(node->getUiPosition().m_y, -15);

    babelwires::MoveNodeCommand testCopyConstructor("Test command", elementId, babelwires::UiPosition{100, 12});
    babelwires::MoveNodeCommand command = testCopyConstructor;
    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_EQ(node->getUiPosition().m_x, 100);
    EXPECT_EQ(node->getUiPosition().m_y, 12);

    command.undo(testEnvironment.m_project);

    EXPECT_EQ(node->getUiPosition().m_x, -14);
    EXPECT_EQ(node->getUiPosition().m_y, -15);

    command.execute(testEnvironment.m_project);

    EXPECT_EQ(node->getUiPosition().m_x, 100);
    EXPECT_EQ(node->getUiPosition().m_y, 12);
}

TEST(MoveNodeCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueAssignmentData modData(babelwires::IntValue(86));
    modData.m_targetPath = babelwires::Path::deserializeFromString("qqq/zzz");

    babelwires::MoveNodeCommand command("Test command", 57, babelwires::UiPosition{100, 12});

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(MoveNodeCommandTest, subsumeMoves) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestNodeData elementData;
    elementData.m_uiData.m_uiPosition = babelwires::UiPosition{-14, -15};

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const testUtils::TestNode* node =
        testEnvironment.m_project.getNode(elementId)->tryAs<testUtils::TestNode>();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getUiPosition().m_x, -14);
    EXPECT_EQ(node->getUiPosition().m_y, -15);

    babelwires::MoveNodeCommand firstCommand("Test command", elementId, babelwires::UiPosition{100, 12});

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    auto secondCommand =
        std::make_unique<babelwires::MoveNodeCommand>("Test Move", elementId, babelwires::UiPosition{14, 88});

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, true));

    firstCommand.subsume(std::move(secondCommand));

    // Confirm that the move was subsumed
    firstCommand.undo(testEnvironment.m_project);

    EXPECT_EQ(node->getUiPosition().m_x, -14);
    EXPECT_EQ(node->getUiPosition().m_y, -15);

    firstCommand.execute(testEnvironment.m_project);

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getUiPosition().m_x, 14);
    EXPECT_EQ(node->getUiPosition().m_y, 88);
}

TEST(MoveNodeCommandTest, subsumeMovesDelay) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestNodeData elementData;
    elementData.m_uiData.m_uiPosition = babelwires::UiPosition{-14, -15};

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const testUtils::TestNode* node =
        testEnvironment.m_project.getNode(elementId)->tryAs<testUtils::TestNode>();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getUiPosition().m_x, -14);
    EXPECT_EQ(node->getUiPosition().m_y, -15);

    babelwires::MoveNodeCommand firstCommand("Test command", elementId, babelwires::UiPosition{100, 12});

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    auto secondCommand =
        std::make_unique<babelwires::MoveNodeCommand>("Test Move", elementId, babelwires::UiPosition{14, 88});
    secondCommand->setTimestamp(firstCommand.getTimestamp() +
                                babelwires::CommandTimestamp::duration(std::chrono::seconds(2)));

    EXPECT_FALSE(firstCommand.shouldSubsume(*secondCommand, true));
}
