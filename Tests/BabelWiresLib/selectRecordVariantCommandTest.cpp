#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/selectRecordVariantCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
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

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestRecordWithVariantsElementData());
    const babelwires::ElementId sourceId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestSimpleRecordElementData());
    const babelwires::ElementId targetId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestSimpleRecordElementData());

    const babelwires::ValueElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<babelwires::ValueElement>();
    ASSERT_NE(targetElement, nullptr);

    const auto getSelectedTag = [](const babelwires::ValueTreeNode* valueFeature) {
        const auto& type = valueFeature->getType().is<testUtils::TestRecordWithVariantsType>();
        return type.getSelectedTag(valueFeature->getValue());
    };

    ASSERT_NE(element->getInput(), nullptr);

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

    const auto checkModifiers = [&testEnvironment, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection = element->findModifier(testUtils::TestRecordWithVariantsElementData::getPathToFieldA1_Int0());
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(testUtils::TestSimpleRecordElementData::getPathToRecordInt0());
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
            EXPECT_EQ(numModifiersAtElement, 2);
            EXPECT_EQ(numModifiersAtTarget, 0);
        } else {
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 4);
            EXPECT_EQ(numModifiersAtTarget, 1);
        }
    };

    babelwires::SelectRecordVariantCommand command("Test command", elementId, testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants(),
                                                   testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_EQ(getSelectedTag(element->getInput()), testUtils::TestRecordWithVariantsType::getTagAId());
    checkModifiers(false);

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(getSelectedTag(element->getInput()), testUtils::TestRecordWithVariantsType::getTagBId());
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getSelectedTag(element->getInput()), testUtils::TestRecordWithVariantsType::getTagAId());
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getSelectedTag(element->getInput()), testUtils::TestRecordWithVariantsType::getTagBId());
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

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyNotATag) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestRecordWithVariantsElementData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);

    babelwires::ShortId notATag("notTag");
    notATag.setDiscriminator(1);
    babelwires::SelectRecordVariantCommand command("Test command", 51, testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants(), notATag);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyAlreadySelected) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestRecordWithVariantsElementData());

    babelwires::SelectRecordVariantCommand command("Test command", elementId, testUtils::TestRecordWithVariantsElementData::getPathToRecordWithVariants(),
                                                   testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
