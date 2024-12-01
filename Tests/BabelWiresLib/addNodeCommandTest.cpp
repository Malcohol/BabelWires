#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addNodeCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(AddNodeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::AddNodeCommand command("Test command", std::make_unique<testUtils::TestSimpleRecordElementData>());

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);

    const babelwires::Node* newElement = testEnvironment.m_project.getNode(command.getNodeId());
    ASSERT_NE(newElement, nullptr);
    EXPECT_NE(newElement->as<babelwires::ValueNode>(), nullptr);

    command.undo(testEnvironment.m_project);

    EXPECT_EQ(testEnvironment.m_project.getNode(command.getNodeId()), nullptr);

    command.execute(testEnvironment.m_project);

    const babelwires::Node* restoredElement = testEnvironment.m_project.getNode(command.getNodeId());
    ASSERT_NE(restoredElement, nullptr);
    EXPECT_NE(restoredElement->as<babelwires::ValueNode>(), nullptr);
}

TEST(AddNodeCommandTest, subsumeMoves) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::AddNodeCommand addCommand("Test command", std::make_unique<testUtils::TestSimpleRecordElementData>());

    EXPECT_TRUE(addCommand.initializeAndExecute(testEnvironment.m_project));

    auto moveCommand = std::make_unique<babelwires::MoveNodeCommand>("Test Move", addCommand.getNodeId(),
                                                                        babelwires::UiPosition{14, 88});

    EXPECT_TRUE(addCommand.shouldSubsume(*moveCommand, true));

    addCommand.subsume(std::move(moveCommand));

    // Confirm that the move was subsumed
    addCommand.undo(testEnvironment.m_project);
    addCommand.execute(testEnvironment.m_project);
    const auto* element =
        testEnvironment.m_project.getNode(addCommand.getNodeId())->as<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, 14);
    EXPECT_EQ(element->getUiPosition().m_y, 88);
}
