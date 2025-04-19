#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addNodeForOutputTreeValueCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectDataWithCompoundConnection.hpp>

class AddNodeForOutputTreeValueCommandTest : public testing::TestWithParam<babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes> {};

TEST_P(AddNodeForOutputTreeValueCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectDataWithCompoundConnection projectData;
    testEnvironment.m_project.setProjectData(projectData);

    const babelwires::Node* const sourceNode = testEnvironment.m_project.getNode(projectData.m_sourceNodeId);
    ASSERT_NE(sourceNode, nullptr);
    // Expected later.
    ASSERT_TRUE(sourceNode->isExpanded(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord()));

    const babelwires::Node* const targetNode = testEnvironment.m_project.getNode(projectData.m_targetNodeId);
    ASSERT_NE(targetNode, nullptr);

    babelwires::AddNodeForOutputTreeValueCommand testCopyConstructor(
        "test command", projectData.m_sourceNodeId, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(),
        {-10, -20}, GetParam());
    babelwires::AddNodeForOutputTreeValueCommand command = testCopyConstructor;
    
    EXPECT_EQ(command.getName(), "test command");
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    const babelwires::NodeId newNodeId = command.getNodeId();
    EXPECT_NE(newNodeId, babelwires::INVALID_NODE_ID);

    auto testWhenExecuted = [&] () {
        const babelwires::Node* const newNode = testEnvironment.m_project.getNode(newNodeId);
        ASSERT_NE(newNode, nullptr);
        EXPECT_NE(newNode->as<babelwires::ValueNode>(), nullptr);

        {
            const babelwires::Modifier* const modifierAtNewNode = newNode->findModifier(babelwires::Path());
            ASSERT_NE(modifierAtNewNode, nullptr);

            const babelwires::ConnectionModifier* const connectionAtNewNode = modifierAtNewNode->as<babelwires::ConnectionModifier>();
            ASSERT_NE(connectionAtNewNode, nullptr);

            const babelwires::ConnectionModifierData& connectionData = connectionAtNewNode->getModifierData();
            EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);
            EXPECT_EQ(connectionData.m_sourcePath, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
        }

        EXPECT_TRUE(newNode->isExpanded(babelwires::Path()));

        {
            const babelwires::Modifier* const modifierAtTargetNode = targetNode->findModifier(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
            ASSERT_NE(modifierAtTargetNode, nullptr);

            const babelwires::ConnectionModifier* const connectionAtTargetNode = modifierAtTargetNode->as<babelwires::ConnectionModifier>();
            ASSERT_NE(connectionAtTargetNode, nullptr);

            const babelwires::ConnectionModifierData& connectionData = connectionAtTargetNode->getModifierData();

            if (GetParam() == babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::NewParent) {
                EXPECT_EQ(connectionData.m_sourcePath, babelwires::Path());
                EXPECT_EQ(connectionData.m_sourceId, newNodeId);
            } else {
                EXPECT_EQ(connectionData.m_sourcePath, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
                EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);
            }
        }
    };
    testWhenExecuted();

    command.undo(testEnvironment.m_project);

    ASSERT_EQ(testEnvironment.m_project.getNode(newNodeId), nullptr);
    {
        const babelwires::Modifier* const modifierAtTargetNode = targetNode->findModifier(testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
        ASSERT_NE(modifierAtTargetNode, nullptr);

        const babelwires::ConnectionModifier* const connectionAtTargetNode = modifierAtTargetNode->as<babelwires::ConnectionModifier>();
        ASSERT_NE(connectionAtTargetNode, nullptr);

        const babelwires::ConnectionModifierData& connectionData = connectionAtTargetNode->getModifierData();
        EXPECT_EQ(connectionData.m_sourcePath, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord());
        EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);
    }

    command.execute(testEnvironment.m_project);

    testWhenExecuted();
}

TEST_P(AddNodeForOutputTreeValueCommandTest, subsumeMoves) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectDataWithCompoundConnection projectData;
    testEnvironment.m_project.setProjectData(projectData);

    babelwires::AddNodeForOutputTreeValueCommand command(
        "test command", projectData.m_sourceNodeId, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(),
        {-10, -20}, GetParam());

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    auto moveCommand = std::make_unique<babelwires::MoveNodeCommand>("Test Move", command.getNodeId(),
                                                                        babelwires::UiPosition{14, 88});

    EXPECT_TRUE(command.shouldSubsume(*moveCommand, true));

    command.subsume(std::move(moveCommand));

    // Confirm that the move was subsumed
    command.undo(testEnvironment.m_project);
    command.execute(testEnvironment.m_project);
    const auto* element =
        testEnvironment.m_project.getNode(command.getNodeId())->as<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, 14);
    EXPECT_EQ(element->getUiPosition().m_y, 88);
}

TEST_P(AddNodeForOutputTreeValueCommandTest, failSafelyNoSourceNode) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::AddNodeForOutputTreeValueCommand command(
        "test command", 32, testDomain::TestComplexRecordElementData::getPathToRecordSubrecord(),
        {-10, -20}, GetParam());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST_P(AddNodeForOutputTreeValueCommandTest, failSafelyNoSourceValue) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectDataWithCompoundConnection projectData;
    testEnvironment.m_project.setProjectData(projectData);

    babelwires::AddNodeForOutputTreeValueCommand command(
        "test command", projectData.m_sourceNodeId, babelwires::Path::deserializeFromString("aaa/bbb"),
        {-10, -20}, GetParam());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

INSTANTIATE_TEST_SUITE_P(
    AddNodeForOutputTreeValueCommandTest, AddNodeForOutputTreeValueCommandTest,
    testing::Values(babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::NewParent, babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::Sibling)
);
