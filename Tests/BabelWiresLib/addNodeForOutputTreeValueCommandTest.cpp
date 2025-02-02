#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addNodeForOutputTreeValueCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectDataWithCompoundConnection.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

class AddNodeForOutputTreeValueCommandTest : public testing::TestWithParam<babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes> {};

TEST_P(AddNodeForOutputTreeValueCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectDataWithCompoundConnection projectData;
    testEnvironment.m_project.setProjectData(projectData);

    const babelwires::Node* const sourceNode = testEnvironment.m_project.getNode(projectData.m_sourceNodeId);
    ASSERT_NE(sourceNode, nullptr);
    // Expected later.
    ASSERT_TRUE(sourceNode->isExpanded(testUtils::TestComplexRecordElementData::getPathToRecord()));

    const babelwires::Node* const targetNode = testEnvironment.m_project.getNode(projectData.m_targetNodeId);
    ASSERT_NE(targetNode, nullptr);

    babelwires::AddNodeForOutputTreeValueCommand command(
        "test command", projectData.m_sourceNodeId, testUtils::TestComplexRecordElementData::getPathToRecord(),
        {-10, -20}, GetParam());
    
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
            EXPECT_EQ(connectionData.m_sourcePath, testUtils::TestComplexRecordElementData::getPathToRecord());
        }

        EXPECT_TRUE(newNode->isExpanded(babelwires::Path()));

        {
            const babelwires::Modifier* const modifierAtTargetNode = targetNode->findModifier(babelwires::Path());
            ASSERT_NE(modifierAtTargetNode, nullptr);

            const babelwires::ConnectionModifier* const connectionAtTargetNode = modifierAtTargetNode->as<babelwires::ConnectionModifier>();
            ASSERT_NE(connectionAtTargetNode, nullptr);

            const babelwires::ConnectionModifierData& connectionData = connectionAtTargetNode->getModifierData();
            EXPECT_EQ(connectionData.m_sourcePath, testUtils::TestComplexRecordElementData::getPathToRecord());

            if (GetParam() == babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::NewParent) {
                EXPECT_EQ(connectionData.m_sourceId, newNodeId);
            } else {
                EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);
            }
        }
    };
    testWhenExecuted();

    command.undo(testEnvironment.m_project);

    ASSERT_EQ(testEnvironment.m_project.getNode(newNodeId), nullptr);
    {
        const babelwires::Modifier* const modifierAtTargetNode = targetNode->findModifier(babelwires::Path());
        ASSERT_NE(modifierAtTargetNode, nullptr);

        const babelwires::ConnectionModifier* const connectionAtTargetNode = modifierAtTargetNode->as<babelwires::ConnectionModifier>();
        ASSERT_NE(connectionAtTargetNode, nullptr);

        const babelwires::ConnectionModifierData& connectionData = connectionAtTargetNode->getModifierData();
        EXPECT_EQ(connectionData.m_sourcePath, testUtils::TestComplexRecordElementData::getPathToRecord());
        EXPECT_EQ(connectionData.m_sourceId, projectData.m_sourceNodeId);
    }

    command.execute(testEnvironment.m_project);

    testWhenExecuted();
}

INSTANTIATE_TEST_SUITE_P(
    AddNodeForOutputTreeValueCommandTest, AddNodeForOutputTreeValueCommandTest,
    testing::Values(babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::NewParent, babelwires::AddNodeForOutputTreeValueCommand::RelationshipToDependentNodes::Sibling)
);
