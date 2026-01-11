#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Project/Commands/setTypeVariableCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <Domains/TestDomain/testGenericType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    void checkInstantiation (const babelwires::Project& project, babelwires::NodeId genericNodeId, bool var0Instantiated, bool var1Instantiated) {
        const babelwires::Node *const genericNode = project.getNode(genericNodeId);
        ASSERT_NE(genericNode, nullptr);

        const babelwires::ValueTreeNode *const input = genericNode->getInput();
        ASSERT_NE(input, nullptr);

        const testDomain::TestGenericType *const genericType = input->getType()->tryAs<testDomain::TestGenericType>();
        ASSERT_NE(genericType, nullptr);

        const babelwires::TypeExp& typeAssignment0 = genericType->getTypeAssignment(input->getValue(), 0);
        if (var0Instantiated) {
            EXPECT_EQ(typeAssignment0, babelwires::StringType::getThisIdentifier());
        } else {
            EXPECT_EQ(typeAssignment0, babelwires::TypeExp());
        }

        const babelwires::TypeExp& typeAssignment1 = genericType->getTypeAssignment(input->getValue(), 1);
        if (var1Instantiated) {
            EXPECT_EQ(typeAssignment1, babelwires::StringType::getThisIdentifier());
        } else {
            EXPECT_EQ(typeAssignment1, babelwires::TypeExp());
        }
    };
}

TEST(SetTypeVariableCommandTest, setSingleTypeVariable) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;

    const babelwires::NodeId genericNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestGenericType::getThisIdentifier()));
    
    babelwires::SetTypeVariableCommand testCopyConstructor("Test command", genericNodeId, babelwires::Path(), 0, babelwires::StringType::getThisIdentifier());
    babelwires::SetTypeVariableCommand command = testCopyConstructor;
    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    checkInstantiation(testEnvironment.m_project, genericNodeId, false, false);

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false);

    command.undo(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, false, false);

    command.execute(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false);
}

TEST(SetTypeVariableCommandTest, setAnotherTypeVariable) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::TypeSystem& typeSystem = testEnvironment.m_typeSystem;

    const babelwires::NodeId genericNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestGenericType::getThisIdentifier()));
    
    babelwires::SetTypeVariableCommand setFirstVariable("Test command", genericNodeId, babelwires::Path(), 0, babelwires::StringType::getThisIdentifier());
    babelwires::SetTypeVariableCommand setSecondVariable("Test command", genericNodeId, babelwires::Path(), 1, babelwires::StringType::getThisIdentifier());

    testEnvironment.m_project.process();
    checkInstantiation(testEnvironment.m_project, genericNodeId, false, false);

    EXPECT_TRUE(setFirstVariable.initializeAndExecute(testEnvironment.m_project));
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false);

    EXPECT_TRUE(setSecondVariable.initializeAndExecute(testEnvironment.m_project));
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, true);

    setSecondVariable.undo(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, false);

    setSecondVariable.execute(testEnvironment.m_project);
    checkInstantiation(testEnvironment.m_project, genericNodeId, true, true);
}

TEST(SetTypeVariableCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SetTypeVariableCommand command("Test command", 13, babelwires::Path(), 0, babelwires::StringType::getThisIdentifier());
    
    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SetTypeVariableCommandTest, failSafelyNoGenericType) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisIdentifier()));

    babelwires::SetTypeVariableCommand command("Test command", elementId, babelwires::Path(), 0, babelwires::StringType::getThisIdentifier());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SetTypeVariableCommandTest, failSafelyVariableOutOfRange) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId genericNodeId = testEnvironment.m_project.addNode(babelwires::ValueNodeData(testDomain::TestGenericType::getThisIdentifier()));
    
    babelwires::SetTypeVariableCommand command("Test command", genericNodeId, babelwires::Path(), 3, babelwires::StringType::getThisIdentifier());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

