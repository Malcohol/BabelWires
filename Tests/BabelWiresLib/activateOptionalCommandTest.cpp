#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/activateOptionalCommand.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(ActivateOptionalsCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testUtils::TestComplexRecordType::getThisType()));
    const babelwires::ValueNode* const element =
        testEnvironment.m_project.getNode(elementId)->as<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);

    const babelwires::ValueTreeNode* const input = element->getInput();
    ASSERT_NE(input, nullptr);
    const testUtils::TestComplexRecordType* const type = input->getType().as<testUtils::TestComplexRecordType>();

    const babelwires::Path pathToValue;

    babelwires::ActivateOptionalCommand command("Test command", elementId, pathToValue,
                                                testUtils::TestComplexRecordType::getOpRecId());

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(type->isActivated(input->getValue(), testUtils::TestComplexRecordType::getOpRecId()));

    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(pathToValue);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(type->isActivated(input->getValue(), testUtils::TestComplexRecordType::getOpRecId()));
    EXPECT_EQ(element->getEdits().findModifier(pathToValue), nullptr);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(type->isActivated(input->getValue(), testUtils::TestComplexRecordType::getOpRecId()));

    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(pathToValue);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }
}

TEST(ActivateOptionalsCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                                babelwires::Path(), opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                                babelwires::Path::deserializeFromString("qqq/zzz"), opId);

    babelwires::ValueNodeData elementData(testUtils::TestComplexRecordType::getThisType());
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addNode(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testUtils::TestComplexRecordType::getThisType()));

    const babelwires::Path pathToValue;

    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                                pathToValue, opId);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testUtils::TestComplexRecordType::getThisType()));

    // Not an optional field
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);

    // Subrecord is a TestSimpleRecordType and has no optionals.
    const babelwires::Path pathToValue;

    babelwires::ActivateOptionalCommand command(
        "Test command", elementId, pathToValue, opId);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyAlreadyActivated) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addNode(
        babelwires::ValueNodeData(testUtils::TestComplexRecordType::getThisType()));
    babelwires::ValueNode* const element =
        testEnvironment.m_project.getNode(elementId)->as<babelwires::ValueNode>();
    ASSERT_NE(element, nullptr);

    const babelwires::Path pathToValue;

    auto* const inputRecord = element->getInputNonConst(pathToValue);
    ASSERT_NE(inputRecord, nullptr);

    // Active the optional first.
    testUtils::TestComplexRecordType::Instance instance{*inputRecord};
    instance.activateAndGetopInt();

    babelwires::ActivateOptionalCommand command(
        "Test command", elementId, pathToValue, testUtils::TestComplexRecordType::getOpIntId());

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
