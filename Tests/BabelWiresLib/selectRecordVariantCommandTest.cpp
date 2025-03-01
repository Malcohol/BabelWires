#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/selectRecordVariantCommand.hpp>

#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordWithVariantsType.hpp>

TEST(SelectRecordVariantCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestRecordWithVariantsElementData());
    const babelwires::NodeId sourceId =
        testEnvironment.m_project.addNode(testUtils::TestSimpleRecordElementData());
    const babelwires::NodeId targetId =
        testEnvironment.m_project.addNode(testUtils::TestSimpleRecordElementData());

    const babelwires::ValueNode* node =
        testEnvironment.m_project.getNode(elementId)->as<babelwires::ValueNode>();
    ASSERT_NE(node, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getNode(targetId)->as<babelwires::ValueNode>();
    ASSERT_NE(targetElement, nullptr);

    const auto getSelectedTag = [](const babelwires::ValueTreeNode* valueTreeNode) {
        const auto& type = valueTreeNode->getType().is<testUtils::TestRecordWithVariantsType>();
        return type.getSelectedTag(valueTreeNode->getValue());
    };

    ASSERT_NE(node->getInput(), nullptr);

    {
        babelwires::SelectRecordVariantModifierData selectRecordVariantData;
        selectRecordVariantData.m_targetPath = testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants();
        selectRecordVariantData.m_tagToSelect = testUtils::TestRecordWithVariantsType::getTagAId();
        testEnvironment.m_project.addModifier(elementId, selectRecordVariantData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_targetPath = testUtils::TestRecordWithVariantsElementData::getPathToFieldA1_Int0();
        inputConnection.m_sourcePath = testUtils::TestSimpleRecordElementData::getPathToRecordInt0();
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_targetPath = testUtils::TestSimpleRecordElementData::getPathToRecordInt0();
        outputConnection.m_sourcePath = testUtils::TestRecordWithVariantsElementData::getPathToFieldA0();
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    {
        babelwires::ValueAssignmentData assignInt(babelwires::IntValue(12));
        assignInt.m_targetPath = testUtils::TestRecordWithVariantsElementData::getPathToFieldAB();
        testEnvironment.m_project.addModifier(elementId, assignInt);
    }
    {
        babelwires::ValueAssignmentData assignInt(babelwires::IntValue(4));
        assignInt.m_targetPath = testUtils::TestRecordWithVariantsElementData::getPathToFieldA0();
        testEnvironment.m_project.addModifier(elementId, assignInt);
    }

    const auto checkModifiers = [&testEnvironment, node, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection = node->findModifier(testUtils::TestRecordWithVariantsElementData::getPathToFieldA1_Int0());
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(testUtils::TestSimpleRecordElementData::getPathToRecordInt0());
        int numModifiersAtElement = 0;
        int numModifiersAtTarget = 0;
        for (const auto* m : node->getEdits().modifierRange()) {
            ++numModifiersAtElement;
        }
        for (const auto* m : targetElement->getEdits().modifierRange()) {
            ++numModifiersAtTarget;
        }
        if (isCommandExecuted) {
            EXPECT_EQ(inputConnection, nullptr);
            EXPECT_EQ(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 2);
            EXPECT_EQ(numModifiersAtTarget, 0);
        } else {
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 4);
            EXPECT_EQ(numModifiersAtTarget, 1);
        }
    };

    babelwires::SelectRecordVariantCommand testCopyConstructor("Test command", elementId, testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants(),
                                                   testUtils::TestRecordWithVariantsType::getTagBId());
    babelwires::SelectRecordVariantCommand command = testCopyConstructor;

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_EQ(getSelectedTag(node->getInput()), testUtils::TestRecordWithVariantsType::getTagAId());
    checkModifiers(false);

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(getSelectedTag(node->getInput()), testUtils::TestRecordWithVariantsType::getTagBId());
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getSelectedTag(node->getInput()), testUtils::TestRecordWithVariantsType::getTagAId());
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getSelectedTag(node->getInput()), testUtils::TestRecordWithVariantsType::getTagBId());
    checkModifiers(true);
}

TEST(SelectRecordVariantCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectRecordVariantCommand command("Test command", 51, testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants(), "tag");

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectRecordVariantCommand command("Test command", 51,
                                                   babelwires::Path::deserializeFromString("qqq/zzz"), "tag");

    testUtils::TestRecordWithVariantsElementData elementData;
    elementData.m_id = 51;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyNotATag) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestRecordWithVariantsElementData());

    const auto* node = testEnvironment.m_project.getNode(elementId)->as<babelwires::ValueNode>();
    ASSERT_NE(node, nullptr);

    babelwires::ShortId notATag("notTag");
    notATag.setDiscriminator(1);
    babelwires::SelectRecordVariantCommand command("Test command", 51, testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants(), notATag);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyAlreadySelected) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestRecordWithVariantsElementData());

    babelwires::SelectRecordVariantCommand command("Test command", elementId, testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants(),
                                                   testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
