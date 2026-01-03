#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Project/Commands/addConnectionCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <Domains/TestDomain/testGenericType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(AddConnectionCommandTest, addConnection) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId targetNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));
    const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));

    babelwires::ValueAssignmentData assignSourceData(babelwires::IntValue(13));
    assignSourceData.m_targetPath = testDomain::TestSimpleRecordElementData().getPathToRecordInt0();
    testEnvironment.m_project.addModifier(sourceNodeId, assignSourceData);

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = testDomain::TestSimpleRecordElementData().getPathToRecordInt1();
    connectionData.m_sourcePath = testDomain::TestSimpleRecordElementData().getPathToRecordInt0();
    connectionData.m_sourceId = sourceNodeId;
    babelwires::AddConnectionCommand testCopyConstructor("Test command", targetNodeId, connectionData.clone());
    babelwires::AddConnectionCommand command = testCopyConstructor;
    EXPECT_EQ(command.getName(), "Test command");

    const auto checkTargetInt = [&](bool isAssigned) {
        const babelwires::Node* const targetNode = testEnvironment.m_project.getNode(targetNodeId);
        ASSERT_NE(targetNode, nullptr);

        const babelwires::ValueTreeNode* const targetInput = targetNode->getInput();
        ASSERT_NE(targetInput, nullptr);

        const babelwires::ValueTreeNode& intTreeNode = babelwires::followPath(testDomain::TestSimpleRecordElementData().getPathToRecordInt1(), *targetInput);
        const babelwires::IntValue* const intValue = intTreeNode.getValue()->as<babelwires::IntValue>();
        ASSERT_NE(intValue, nullptr);

        const babelwires::Modifier* const modifier = targetNode->findModifier(testDomain::TestSimpleRecordElementData().getPathToRecordInt1());

        if (isAssigned) {
            EXPECT_NE(modifier, nullptr);
            EXPECT_EQ(intValue->get(), 13);
        } else {
            EXPECT_EQ(modifier, nullptr);
            EXPECT_EQ(intValue->get(), 0);
        }
    };

    testEnvironment.m_project.process();
    checkTargetInt(false);

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    testEnvironment.m_project.process();
    checkTargetInt(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();
    checkTargetInt(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();
    checkTargetInt(true);
}

TEST(AddConnectionCommandTest, failSafelyNoTargetElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = babelwires::Path();
    connectionData.m_sourcePath = babelwires::Path();
    connectionData.m_sourceId = 51;
    babelwires::AddConnectionCommand command("Test command", 13, connectionData.clone());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddConnectionCommandTest, failSafelyNoSourceElement) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId targetNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = babelwires::Path();
    connectionData.m_sourcePath = babelwires::Path();
    connectionData.m_sourceId = 51;
    babelwires::AddConnectionCommand command("Test command", targetNodeId, connectionData.clone());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddConnectionCommandTest, failSafelyNoTargetPath) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId targetNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));
    const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = babelwires::Path::deserializeFromString("qqq/zzz");
    connectionData.m_sourcePath = babelwires::Path();
    connectionData.m_sourceId = sourceNodeId;
    babelwires::AddConnectionCommand command("Test command", targetNodeId, connectionData.clone());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddConnectionCommandTest, failSafelyNoSourcePath) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId targetNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));
    const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = babelwires::Path();
    connectionData.m_sourcePath = babelwires::Path::deserializeFromString("qqq/zzz");
    connectionData.m_sourceId = sourceNodeId;
    babelwires::AddConnectionCommand command("Test command", targetNodeId, connectionData.clone());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}  

namespace {
    void checkInstantiation (const babelwires::Project& project, babelwires::NodeId genericNodeId, bool var0Instantiated, bool var1Instantiated, bool expectedModifier) {
        const babelwires::Node *const genericNode = project.getNode(genericNodeId);
        ASSERT_NE(genericNode, nullptr);

        const babelwires::ValueTreeNode *const input = genericNode->getInput();
        ASSERT_NE(input, nullptr);

        const testDomain::TestGenericType *const genericType = input->getType()->as<testDomain::TestGenericType>();
        ASSERT_NE(genericType, nullptr);

        const babelwires::TypeExp& typeAssignment0 = genericType->getTypeAssignment(input->getValue(), 0);
        if (var0Instantiated) {
            EXPECT_EQ(typeAssignment0, babelwires::DefaultIntType::getThisType());
        } else {
            EXPECT_EQ(typeAssignment0, babelwires::TypeExp());
        }

        const babelwires::TypeExp& typeAssignment1 = genericType->getTypeAssignment(input->getValue(), 1);
        if (var1Instantiated) {
            EXPECT_EQ(typeAssignment1, testDomain::TestSimpleRecordType::getThisType());
        } else {
            EXPECT_EQ(typeAssignment1, babelwires::TypeExp());
        }

        if (expectedModifier) {
            EXPECT_NE(genericNode->findModifier(babelwires::Path()), nullptr);
        } else {
            EXPECT_EQ(genericNode->findModifier(babelwires::Path()), nullptr);
        }
    };

    void checkInstantiationOfNestedType(const babelwires::Project& project, babelwires::NodeId genericNodeId, bool varInstantiated, bool expectedModifier) {
        const babelwires::Node *const genericNode = project.getNode(genericNodeId);
        ASSERT_NE(genericNode, nullptr);

        const babelwires::ValueTreeNode *const input = genericNode->getInput();
        ASSERT_NE(input, nullptr);

        const babelwires::ValueTreeNode *const nestedGenericNodeTree = babelwires::tryFollowPath(testDomain::TestGenericType::getPathToNestedGenericType(), *input);
        ASSERT_NE(nestedGenericNodeTree, nullptr);

        const babelwires::GenericType *const genericType = nestedGenericNodeTree->getType()->as<babelwires::GenericType>();
        ASSERT_NE(genericType, nullptr);

        const babelwires::TypeExp& typeAssignment0 = genericType->getTypeAssignment(nestedGenericNodeTree->getValue(), 0);
        if (varInstantiated) {
            EXPECT_EQ(typeAssignment0, babelwires::StringType::getThisType());
        } else {
            EXPECT_EQ(typeAssignment0, babelwires::TypeExp());
        }

        if (expectedModifier) {
            EXPECT_NE(genericNode->findModifier(testDomain::TestGenericType::getPathToNestedGenericType()), nullptr);
        } else {
            EXPECT_EQ(genericNode->findModifier(testDomain::TestGenericType::getPathToNestedGenericType()), nullptr);
        }
    };

}

TEST(AddConnectionCommandTest, connectToTypeVariable) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;

    const babelwires::NodeId genericNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestGenericType::getThisType()));
    const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = testDomain::TestGenericType::getPathToNestedX();
    connectionData.m_sourcePath = testDomain::TestSimpleRecordElementData().getPathToRecordInt0();
    connectionData.m_sourceId = sourceNodeId;
    babelwires::AddConnectionCommand testCopyConstructor("Test command", genericNodeId, connectionData.clone());
    babelwires::AddConnectionCommand command = testCopyConstructor;
    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    checkInstantiation(testEnvironment.m_project, genericNodeId, false, false, false);

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false, true);

    command.undo(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, false, false, false);

    command.execute(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false, true);
}

TEST(AddConnectionCommandTest, connectToAnotherTypeVariable) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;

    const babelwires::NodeId genericNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestGenericType::getThisType()));
    const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));
    testEnvironment.m_project.process();

    babelwires::SetTypeVariableModifierData setFirstVariableData;
    setFirstVariableData.m_targetPath = babelwires::Path();
    setFirstVariableData.m_typeAssignments.push_back(babelwires::DefaultIntType::getThisType());
    setFirstVariableData.m_typeAssignments.push_back(babelwires::TypeExp());
    testEnvironment.m_project.addModifier(genericNodeId, setFirstVariableData);
    testEnvironment.m_project.process();
    
    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = testDomain::TestGenericType::getPathToY();
    connectionData.m_sourcePath = babelwires::Path();
    connectionData.m_sourceId = sourceNodeId;
    babelwires::AddConnectionCommand command("Test command", genericNodeId, connectionData.clone());

    testEnvironment.m_project.process();
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false, true);

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, true, true);

    command.undo(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false, true);

    command.execute(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, true, true);
}

TEST(AddConnectionCommandTest, compoundConnectionToGenericType) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;

    const babelwires::NodeId genericNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestGenericType::getThisType()));
    const babelwires::NodeId sourceNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleCompoundType::getThisType()));
    testEnvironment.m_project.process();

    babelwires::ConnectionModifierData connectionData;
    connectionData.m_targetPath = testDomain::TestGenericType::getPathToNestedWrappedType();
    connectionData.m_sourcePath = babelwires::Path();
    connectionData.m_sourceId = sourceNodeId;
    babelwires::AddConnectionCommand command("Test command", genericNodeId, connectionData.clone());

    checkInstantiation(testEnvironment.m_project, genericNodeId, false, false, false);   
    checkInstantiationOfNestedType(testEnvironment.m_project, genericNodeId, false, false);

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false, true);   
    checkInstantiationOfNestedType(testEnvironment.m_project, genericNodeId, true, true);

    command.undo(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, false, false, false);   
    checkInstantiationOfNestedType(testEnvironment.m_project, genericNodeId, false, false);

    command.execute(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false, true);   
    checkInstantiationOfNestedType(testEnvironment.m_project, genericNodeId, true, true);
}
