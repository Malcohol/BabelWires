#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/setArraySizeCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testArrayType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

// TODO Test default array with non-minimum default size.

TEST(SetArraySizeCommandTest, executeAndUndoArrayGrow) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testDomain::TestArrayElementData());
    const babelwires::NodeId sourceId = testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());
    const babelwires::NodeId targetId = testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());

    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_targetPath = testDomain::TestArrayElementData::getPathToArray();
        arrayInitialization.m_size = 3;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_targetPath = testDomain::TestArrayElementData::getPathToArray_1();
        inputConnection.m_sourcePath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_targetPath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
        outputConnection.m_sourcePath = testDomain::TestArrayElementData::getPathToArray_2();
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(element, nullptr);
    const auto* targetElement = testEnvironment.m_project.getNode(targetId);
    ASSERT_NE(targetElement, nullptr);

    const auto checkModifiers = [&testEnvironment, element, targetElement]() {
        const babelwires::Modifier* inputConnection =
            element->findModifier(testDomain::TestArrayElementData::getPathToArray_1());
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(testDomain::TestSimpleRecordElementData::getPathToRecordInt0());
        int numModifiersAtElement = 0;
        int numModifiersAtTarget = 0;
        for (const auto* m : element->getEdits().modifierRange()) {
            ++numModifiersAtElement;
        }
        for (const auto* m : targetElement->getEdits().modifierRange()) {
            ++numModifiersAtTarget;
        }
        EXPECT_NE(inputConnection, nullptr);
        EXPECT_NE(outputConnection, nullptr);
        EXPECT_EQ(numModifiersAtElement, 2);
        EXPECT_EQ(numModifiersAtTarget, 1);
    };

    ASSERT_NE(element->getInput(), nullptr);

    EXPECT_EQ(element->getInput()->getNumChildren(), 3);
    checkModifiers();

    babelwires::SetArraySizeCommand testCopyConstructor("Test command", elementId,
                                                        testDomain::TestArrayElementData::getPathToArray(), 5);
    babelwires::SetArraySizeCommand command = testCopyConstructor;

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), 5);
    checkModifiers();

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), 3);
    checkModifiers();

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), 5);
    checkModifiers();
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayShrink) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testDomain::TestArrayElementData());
    const babelwires::NodeId sourceId = testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());
    const babelwires::NodeId targetId = testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());

    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_targetPath = testDomain::TestArrayElementData::getPathToArray();
        arrayInitialization.m_size = 3;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_targetPath = testDomain::TestArrayElementData::getPathToArray_1();
        inputConnection.m_sourcePath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_targetPath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
        outputConnection.m_sourcePath = testDomain::TestArrayElementData::getPathToArray_2();
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(element, nullptr);
    const auto* targetElement = testEnvironment.m_project.getNode(targetId);
    ASSERT_NE(targetElement, nullptr);

    const auto checkModifiers = [&testEnvironment, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(testDomain::TestArrayElementData::getPathToArray_1());
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(testDomain::TestSimpleRecordElementData::getPathToRecordInt0());
        int numModifiersAtElement = 0;
        int numModifiersAtTarget = 0;
        for (const auto* m : element->getEdits().modifierRange()) {
            ++numModifiersAtElement;
        }
        for (const auto* m : targetElement->getEdits().modifierRange()) {
            ++numModifiersAtTarget;
        }
        if (isCommandExecuted) {
            EXPECT_EQ(inputConnection, nullptr);
            EXPECT_EQ(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 1);
            EXPECT_EQ(numModifiersAtTarget, 0);
        } else {
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 2);
            EXPECT_EQ(numModifiersAtTarget, 1);
        }
    };

    ASSERT_NE(element->getInput(), nullptr);

    EXPECT_EQ(element->getInput()->getNumChildren(), 3);
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                            testDomain::TestArrayElementData::getPathToArray(), 1);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), 1);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), 3);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), 1);
    checkModifiers(true);
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayNoPriorModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testDomain::TestArrayElementData());
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(element, nullptr);

    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(testDomain::TestArrayElementData::getPathToArray());
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    ASSERT_NE(element->getInput(), nullptr);

    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                            testDomain::TestArrayElementData::getPathToArray(),
                                            testDomain::TestSimpleArrayType::s_nonDefaultSize);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_nonDefaultSize);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_nonDefaultSize);
    checkModifiers(true);
}

TEST(SetArraySizeCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SetArraySizeCommand command("Test command", 51, *babelwires::Path::deserializeFromString("qqq/zzz"), 4);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SetArraySizeCommandTest, failSafelyNoArray) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                            *babelwires::Path::deserializeFromString("qqq/zzz"), 4);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SetArraySizeCommandTest, failSafelyOutOfRange) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testDomain::TestArrayElementData());
    const auto* element = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(element, nullptr);

    ASSERT_NE(element->getInput(), nullptr);
    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_defaultSize);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                            testDomain::TestArrayElementData::getPathToArray(), 12);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_defaultSize);
}
