#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(AddModifierCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;
    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);
    testUtils::TestComplexRecordType::ConstInstance instance(*node->getInput());

    static_assert(8 != testUtils::TestComplexRecordType::c_int1default);
    EXPECT_EQ(instance.getintR1().get(), testUtils::TestComplexRecordType::c_int1default);
    EXPECT_EQ(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);

    babelwires::ValueAssignmentData modData(babelwires::IntValue(8));
    modData.m_targetPath = elementData.getPathToRecordInt1();

    babelwires::AddModifierCommand testCopyConstructor("Test command", elementId, modData.clone());
    babelwires::AddModifierCommand command = testCopyConstructor;

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    EXPECT_EQ(instance.getintR1().get(), 8);
    EXPECT_NE(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);

    command.undo(testEnvironment.m_project);
    EXPECT_EQ(instance.getintR1().get(), testUtils::TestComplexRecordType::c_int1default);
    EXPECT_EQ(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);

    command.execute(testEnvironment.m_project);
    EXPECT_EQ(instance.getintR1().get(), 8);
    EXPECT_NE(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);
}

TEST(AddModifierCommandTest, executeAndUndoFail) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;
    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);
    testUtils::TestComplexRecordType::ConstInstance instance(*node->getInput());

    static_assert(12 > testUtils::TestComplexRecordType::c_int1max);
    EXPECT_EQ(instance.getintR1().get(), testUtils::TestComplexRecordType::c_int1default);
    EXPECT_EQ(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);

    babelwires::ValueAssignmentData modData(babelwires::IntValue(12));
    modData.m_targetPath = elementData.getPathToRecordInt1();

    babelwires::AddModifierCommand command("Test command", elementId, modData.clone());
    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    EXPECT_EQ(instance.getintR1().get(), testUtils::TestComplexRecordType::c_int1default);
    EXPECT_NE(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);
    EXPECT_TRUE(node->getEdits().findModifier(elementData.getPathToRecordInt1())->isFailed());

    command.undo(testEnvironment.m_project);
    EXPECT_EQ(instance.getintR1().get(), testUtils::TestComplexRecordType::c_int1default);
    EXPECT_EQ(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);

    command.execute(testEnvironment.m_project);
    EXPECT_EQ(instance.getintR1().get(), testUtils::TestComplexRecordType::c_int1default);
    EXPECT_NE(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);
    EXPECT_TRUE(node->getEdits().findModifier(elementData.getPathToRecordInt1())->isFailed());
}

TEST(AddModifierCommandTest, executeAndUndoPreexistingModifier) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;
    {
        static_assert(5 != testUtils::TestComplexRecordType::c_int1default);
        babelwires::ValueAssignmentData modData(babelwires::IntValue(5));
        modData.m_targetPath = elementData.getPathToRecordInt1();
        elementData.m_modifiers.emplace_back(modData.clone());
    }
    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);
    testUtils::TestComplexRecordType::ConstInstance instance(*node->getInput());
    EXPECT_EQ(instance.getintR1().get(), 5);

    static_assert(8 != testUtils::TestComplexRecordType::c_int1default);
    babelwires::ValueAssignmentData modData(babelwires::IntValue(8));
    modData.m_targetPath = elementData.getPathToRecordInt1();

    babelwires::AddModifierCommand command("Test command", elementId, modData.clone());
    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    EXPECT_EQ(instance.getintR1().get(), 8);
    EXPECT_NE(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);

    command.undo(testEnvironment.m_project);
    EXPECT_EQ(instance.getintR1().get(), 5);
    EXPECT_NE(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);

    command.execute(testEnvironment.m_project);
    EXPECT_EQ(instance.getintR1().get(), 8);
    EXPECT_NE(node->getEdits().findModifier(elementData.getPathToRecordInt1()), nullptr);
}

TEST(AddModifierCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueAssignmentData modData(babelwires::IntValue(86));
    modData.m_targetPath = babelwires::Path::deserializeFromString("qqq/zzz");

    babelwires::AddModifierCommand command("Test command", 51, modData.clone());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddModifierCommandTest, failSafelyNoTarget) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;
    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    testEnvironment.m_project.getNode(elementId);

    babelwires::ValueAssignmentData modData(babelwires::IntValue(86));
    modData.m_targetPath = babelwires::Path::deserializeFromString("qqq/zzz");

    babelwires::AddModifierCommand command("Test command", elementId, modData.clone());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
