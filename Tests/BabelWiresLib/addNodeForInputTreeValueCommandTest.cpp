#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addNodeForInputTreeValueCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectDataWithCompoundConnection.hpp>

class AddNodeForInputTreeValueCommandTest
    : public testing::TestWithParam<babelwires::AddNodeForInputTreeValueCommand::RelationshipToOldNode> {};

TEST_P(AddNodeForInputTreeValueCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectDataWithCompoundConnection projectData;
    testEnvironment.m_project.setProjectData(projectData);

    const babelwires::Node* const sourceNode = testEnvironment.m_project.getNode(projectData.m_sourceNodeId);
    ASSERT_NE(sourceNode, nullptr);

    const babelwires::Node* const targetNode = testEnvironment.m_project.getNode(projectData.m_targetNodeId);
    ASSERT_NE(targetNode, nullptr);
    // Expected later.
    ASSERT_TRUE(targetNode->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    babelwires::AddNodeForInputTreeValueCommand testCopyConstructor(
        "test command", projectData.m_targetNodeId, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(),
        {-10, -20}, GetParam());
    babelwires::AddNodeForInputTreeValueCommand command = testCopyConstructor;

    EXPECT_EQ(command.getName(), "test command");
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    const babelwires::NodeId newNodeId = command.getNodeId();
    EXPECT_NE(newNodeId, babelwires::INVALID_NODE_ID);

    auto checkNodeHasIntModifier = [](const babelwires::Node* node, const babelwires::Path& path) {
        if (const babelwires::Modifier* const modifier = node->findModifier(path)) {
            if (auto data = modifier->getModifierData().tryAs<babelwires::ValueAssignmentData>()) {
                if (data->getValue()->tryAs<babelwires::IntValue>()) {
                    return true;
                }
            }
        }
        return false;
    };

    auto testWhenExecuted = [&]() {
        const babelwires::Node* const newNode = testEnvironment.m_project.getNode(newNodeId);
        ASSERT_NE(newNode, nullptr);
        EXPECT_NE(newNode->tryAs<babelwires::ValueNode>(), nullptr);

        {
            const babelwires::Modifier* const modifierAtNewNode = newNode->findModifier(babelwires::Path());
            ASSERT_NE(modifierAtNewNode, nullptr);

            const babelwires::ConnectionModifier* const connectionAtNewNode =
                modifierAtNewNode->tryAs<babelwires::ConnectionModifier>();
            ASSERT_NE(connectionAtNewNode, nullptr);

            const babelwires::ConnectionModifierData& connectionData = connectionAtNewNode->getModifierData();
            EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);
            EXPECT_EQ(connectionData.m_sourcePath, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
        }

        EXPECT_TRUE(newNode->isExpanded(babelwires::Path()));

        {
            const babelwires::Modifier* const modifierAtTargetNode =
                targetNode->findModifier(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
            ASSERT_NE(modifierAtTargetNode, nullptr);

            const babelwires::ConnectionModifier* const connectionAtTargetNode =
                modifierAtTargetNode->tryAs<babelwires::ConnectionModifier>();
            ASSERT_NE(connectionAtTargetNode, nullptr);

            const babelwires::ConnectionModifierData& connectionData = connectionAtTargetNode->getModifierData();

            if (GetParam() == babelwires::AddNodeForInputTreeValueCommand::RelationshipToOldNode::Source) {
                EXPECT_EQ(connectionData.m_sourcePath, babelwires::Path());
                EXPECT_EQ(connectionData.m_sourceId, newNodeId);
                EXPECT_TRUE(
                    checkNodeHasIntModifier(newNode, testDomain::TestSimpleRecordElementData::getPathToRecordInt1()));
                EXPECT_FALSE(checkNodeHasIntModifier(
                    targetNode, testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt1()));
            } else {
                EXPECT_EQ(connectionData.m_sourcePath,
                          testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
                EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);
                EXPECT_TRUE(
                    checkNodeHasIntModifier(newNode, testDomain::TestSimpleRecordElementData::getPathToRecordInt1()));
                EXPECT_TRUE(checkNodeHasIntModifier(
                    targetNode, testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt1()));
            }
        }
    };
    testWhenExecuted();

    command.undo(testEnvironment.m_project);

    ASSERT_EQ(testEnvironment.m_project.getNode(newNodeId), nullptr);
    {
        const babelwires::Modifier* const modifierAtTargetNode =
            targetNode->findModifier(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
        ASSERT_NE(modifierAtTargetNode, nullptr);

        const babelwires::ConnectionModifier* const connectionAtTargetNode =
            modifierAtTargetNode->tryAs<babelwires::ConnectionModifier>();
        ASSERT_NE(connectionAtTargetNode, nullptr);

        const babelwires::ConnectionModifierData& connectionData = connectionAtTargetNode->getModifierData();
        EXPECT_EQ(connectionData.m_sourcePath, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
        EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);

        EXPECT_TRUE(checkNodeHasIntModifier(targetNode,
                                            testDomain::TestComplexRecordElementData::getPathToRecordSubrecordInt1()));
    }

    command.execute(testEnvironment.m_project);

    testWhenExecuted();
}

TEST_P(AddNodeForInputTreeValueCommandTest, subsumeMoves) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectDataWithCompoundConnection projectData;
    testEnvironment.m_project.setProjectData(projectData);

    babelwires::AddNodeForInputTreeValueCommand command(
        "test command", projectData.m_targetNodeId, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(),
        {-10, -20}, GetParam());

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    auto moveCommand =
        std::make_unique<babelwires::MoveNodeCommand>("Test Move", command.getNodeId(), babelwires::UiPosition{14, 88});

    EXPECT_TRUE(command.shouldSubsume(*moveCommand, true));

    command.subsume(std::move(moveCommand));

    // Confirm that the move was subsumed
    command.undo(testEnvironment.m_project);
    command.execute(testEnvironment.m_project);
    const auto* element = testEnvironment.m_project.getNode(command.getNodeId())->tryAs<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, 14);
    EXPECT_EQ(element->getUiPosition().m_y, 88);
}

TEST_P(AddNodeForInputTreeValueCommandTest, failSafelyNoTargetNode) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::AddNodeForInputTreeValueCommand command(
        "test command", 32, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(), {-10, -20},
        GetParam());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST_P(AddNodeForInputTreeValueCommandTest, failSafelyNoTargetValue) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectDataWithCompoundConnection projectData;
    testEnvironment.m_project.setProjectData(projectData);

    babelwires::AddNodeForInputTreeValueCommand command("test command", projectData.m_sourceNodeId,
                                                        babelwires::Path::deserializeFromString("aaa/bbb"), {-10, -20},
                                                        GetParam());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

INSTANTIATE_TEST_SUITE_P(AddNodeForInputTreeValueCommandTest, AddNodeForInputTreeValueCommandTest,
                         testing::Values(babelwires::AddNodeForInputTreeValueCommand::RelationshipToOldNode::Source,
                                         babelwires::AddNodeForInputTreeValueCommand::RelationshipToOldNode::Copy));
