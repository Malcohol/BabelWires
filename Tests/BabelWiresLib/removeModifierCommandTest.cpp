#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/selectOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testArrayType.hpp>
#include <Domains/TestDomain/testRecordType.hpp>
#include <Domains/TestDomain/testRecordWithVariantsType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(RemoveModifierCommandTest, executeAndUndoArray) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testDomain::TestArrayElementData());
    const babelwires::NodeId sourceId =
        testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());
    const babelwires::NodeId targetId =
        testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());

    const unsigned int initialArraySize = testDomain::TestSimpleArrayType::s_defaultSize + 2;
    const babelwires::Path pathToArrayEntry =
        testDomain::TestArrayElementData::getPathToArrayEntry(testDomain::TestSimpleArrayType::s_defaultSize + 1);

    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_targetPath = testDomain::TestArrayElementData::getPathToArray();
        arrayInitialization.m_size = initialArraySize;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_targetPath = pathToArrayEntry;
        inputConnection.m_sourcePath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_targetPath = testDomain::TestSimpleRecordElementData::getPathToRecordInt0();
        outputConnection.m_sourcePath = pathToArrayEntry;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(element, nullptr);
    const auto* targetElement = testEnvironment.m_project.getNode(targetId);
    ASSERT_NE(targetElement, nullptr);

    const auto checkModifiers = [&testEnvironment, element, targetElement, pathToArrayEntry](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(pathToArrayEntry);
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
            EXPECT_EQ(numModifiersAtElement, 0);
            EXPECT_EQ(numModifiersAtTarget, 0);
        } else {
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 2);
            EXPECT_EQ(numModifiersAtTarget, 1);
        }
    };

    ASSERT_NE(element->getInput(), nullptr);

    EXPECT_EQ(element->getInput()->getNumChildren(), initialArraySize);

    babelwires::RemoveModifierCommand testCopyConstructor("Test command", elementId,
        testDomain::TestArrayElementData::getPathToArray());
    babelwires::RemoveModifierCommand command = testCopyConstructor;

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_defaultSize);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), initialArraySize);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInput()->getNumChildren(), testDomain::TestSimpleArrayType::s_defaultSize);
    checkModifiers(true);
}

TEST(RemoveModifierCommandTest, executeAndUndoOptionals) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestComplexRecordType::getThisIdentifier()));

    const babelwires::NodeId sourceId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisIdentifier()));
    const babelwires::NodeId targetId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisIdentifier()));

    const babelwires::ValueNode* const element =
        testEnvironment.m_project.getNode(elementId)->tryAs<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);

    const babelwires::ValueNode* const targetElement =
        testEnvironment.m_project.getNode(targetId)->tryAs<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);

    const babelwires::Path pathToValue;

    babelwires::Path pathToOptional = pathToValue;
    pathToOptional.pushStep(testDomain::TestComplexRecordType::getOpRecId());

    babelwires::Path pathToIntInOptional = pathToOptional;
    pathToIntInOptional.pushStep(testDomain::TestSimpleRecordType::getInt1Id());

    babelwires::Path pathToIntInSimpleRecord = pathToValue;
    pathToIntInSimpleRecord.pushStep(testDomain::TestSimpleRecordType::getInt1Id());

    {
        babelwires::SelectOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_targetPath = pathToValue;
        activateOptionalsModifierData.setOptionalActivation(testDomain::TestComplexRecordType::getOpRecId(), true);
        testEnvironment.m_project.addModifier(elementId, activateOptionalsModifierData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_targetPath = pathToOptional;
        inputConnection.m_sourcePath = pathToValue;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_targetPath = pathToIntInSimpleRecord;
        outputConnection.m_sourcePath = pathToIntInOptional;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }

    const babelwires::ValueTreeNode* const input = element->getInput();
    ASSERT_NE(input, nullptr);
    const testDomain::TestComplexRecordType* const type = input->getType()->tryAs<testDomain::TestComplexRecordType>();

    babelwires::RemoveModifierCommand command("Test command", elementId, pathToValue);

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_EQ(command.getName(), "Test command");

    const auto checkModifiers = [&testEnvironment, element, targetElement, pathToOptional,
                                 pathToIntInSimpleRecord](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection = element->findModifier(pathToOptional);
        const babelwires::Modifier* outputConnection = targetElement->findModifier(pathToIntInSimpleRecord);
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
            EXPECT_EQ(numModifiersAtElement, 0);
            EXPECT_EQ(numModifiersAtTarget, 0);
        } else {
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 2);
            EXPECT_EQ(numModifiersAtTarget, 1);
        }
    };
    testEnvironment.m_project.process();
    checkModifiers(false);

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_FALSE(type->isActivated(input->getValue(), testDomain::TestComplexRecordType::getOpRecId()));
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(type->isActivated(input->getValue(), testDomain::TestComplexRecordType::getOpRecId()));
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(type->isActivated(input->getValue(), testDomain::TestComplexRecordType::getOpRecId()));
    checkModifiers(true);
}

TEST(RemoveModifierCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveModifierCommand command("Test command", 51,
                                              babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveModifierCommandTest, failSafelyNoModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testDomain::TestSimpleRecordElementData());

    babelwires::RemoveModifierCommand command("Test command", elementId,
                                              babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
