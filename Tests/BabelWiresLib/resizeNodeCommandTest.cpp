#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/resizeNodeCommand.hpp>

#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testNode.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(ResizeNodeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestNodeData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const testUtils::TestNode* node =
        testEnvironment.m_project.getNode(elementId)->tryAs<testUtils::TestNode>();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getUiSize().m_width, 77);

    babelwires::ResizeNodeCommand testCopyConstructor("Test command", elementId, babelwires::UiSize{113});
    babelwires::ResizeNodeCommand command = testCopyConstructor;
    
    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_EQ(node->getUiSize().m_width, 113);

    command.undo(testEnvironment.m_project);

    EXPECT_EQ(node->getUiSize().m_width, 77);

    command.execute(testEnvironment.m_project);

    EXPECT_EQ(node->getUiSize().m_width, 113);
}

TEST(ResizeNodeCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueAssignmentData modData(babelwires::IntValue(86));
    modData.m_targetPath = babelwires::Path::deserializeFromString("qqq/zzz");

    babelwires::ResizeNodeCommand command("Test command", 57, babelwires::UiSize{113});

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ResizeNodeCommandTest, subsumeMoves) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestNodeData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const testUtils::TestNode* node =
        testEnvironment.m_project.getNode(elementId)->tryAs<testUtils::TestNode>();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getUiSize().m_width, 77);

    babelwires::ResizeNodeCommand firstCommand("Test command", elementId, babelwires::UiSize{113});

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    auto secondCommand =
        std::make_unique<babelwires::ResizeNodeCommand>("Test Move", elementId, babelwires::UiSize{188});

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, true));

    firstCommand.subsume(std::move(secondCommand));

    // Confirm that the move was subsumed
    firstCommand.undo(testEnvironment.m_project);

    EXPECT_EQ(node->getUiSize().m_width, 77);

    firstCommand.execute(testEnvironment.m_project);

    EXPECT_EQ(node->getUiSize().m_width, 188);
}

TEST(ResizeNodeCommandTest, subsumeMovesDelay) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestNodeData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const testUtils::TestNode* node =
        testEnvironment.m_project.getNode(elementId)->tryAs<testUtils::TestNode>();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getUiSize().m_width, 77);

    babelwires::ResizeNodeCommand firstCommand("Test command", elementId, babelwires::UiSize{113});

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    auto secondCommand =
        std::make_unique<babelwires::ResizeNodeCommand>("Test Move", elementId, babelwires::UiSize{188});
    secondCommand->setTimestamp(firstCommand.getTimestamp() +
                                babelwires::CommandTimestamp::duration(std::chrono::seconds(2)));

    EXPECT_FALSE(firstCommand.shouldSubsume(*secondCommand, true));
}
