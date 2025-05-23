#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(DeactivateOptionalsCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestComplexRecordType::getThisType()));

    const babelwires::NodeId sourceId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));
    const babelwires::NodeId targetId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestSimpleRecordType::getThisType()));

    const babelwires::ValueNode* const element =
        testEnvironment.m_project.getNode(elementId)->as<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);

    const babelwires::ValueNode* const targetElement =
        testEnvironment.m_project.getNode(targetId)->as<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);

    const babelwires::Path pathToValue;

    babelwires::Path pathToOptional = pathToValue;
    pathToOptional.pushStep(testDomain::TestComplexRecordType::getOpRecId());

    babelwires::Path pathToIntInOptional = pathToOptional;
    pathToIntInOptional.pushStep(testDomain::TestSimpleRecordType::getInt1Id());

    babelwires::Path pathToIntInSimpleRecord = pathToValue;
    pathToIntInSimpleRecord.pushStep(testDomain::TestSimpleRecordType::getInt1Id());

    {
        babelwires::ActivateOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_targetPath = pathToValue;
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(testDomain::TestComplexRecordType::getOpRecId());
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
    const testDomain::TestComplexRecordType* const type = input->getType().as<testDomain::TestComplexRecordType>();

    babelwires::DeactivateOptionalCommand testCopyConstructor("Test command", elementId, pathToValue,
        testDomain::TestComplexRecordType::getOpRecId());
    babelwires::DeactivateOptionalCommand command = testCopyConstructor;

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
            EXPECT_EQ(numModifiersAtElement, 1);
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

TEST(DeactivateOptionalsCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51, babelwires::Path(), opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  babelwires::Path::deserializeFromString("qqq/zzz"), opId);

    babelwires::ValueNodeData elementData(testDomain::TestComplexRecordType::getThisType());
    elementData.m_id = 51;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestComplexRecordType::getThisType()));

    const babelwires::Path pathToValue;

    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", elementId, pathToValue, opId);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestComplexRecordType::getThisType()));

    // Not an optional field
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);

    // Subrecord is a TestSimpleRecordType and has no optionals.
    const babelwires::Path pathToValue;

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, pathToValue, opId);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyAlreadyInactive) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testDomain::TestComplexRecordType::getThisType()));

    const babelwires::Path pathToValue;

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, pathToValue, testDomain::TestComplexRecordType::getOpIntId());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
