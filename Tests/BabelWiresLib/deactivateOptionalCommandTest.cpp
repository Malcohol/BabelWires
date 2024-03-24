#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>

#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(DeactivateOptionalsCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());
    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const testUtils::TestFeatureElementWithOptionals* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(targetElement, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    };

    ASSERT_NE(getInputFeature(), nullptr);

    {
        babelwires::ActivateOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToSubrecord;
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(getInputFeature()->m_op1Id);
        testEnvironment.m_project.addModifier(elementId, activateOptionalsModifierData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1;
        inputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = testUtils::TestFeatureWithOptionals::s_pathToOp1_Int2;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, getInputFeature()->m_op1Id);

    const auto checkModifiers = [&testEnvironment, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(testUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1);
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(testUtils::TestRootFeature::s_pathToInt2);
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

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 3);
    checkModifiers(false);

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    testEnvironment.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
    // Note: We do not remove modifiers when a command happens to restore a value to default, so there's no test here to
    // check whether the modifier is present or not.

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 3);
    // Do check that the modifier is present after undo.
    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(testUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(),
                  nullptr);
    }
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  testUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    testUtils::TestFeatureElementWithOptionalsData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);

    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", elementId,
                                                  testUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_ff0Id);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyAlreadyInactive) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op0Id);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, executeAndUndoOnTypes) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));

    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleRecordType::getThisIdentifier()));
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleRecordType::getThisIdentifier()));

    const babelwires::ValueElement* const element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);

    const babelwires::ValueElement* const targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);

    const babelwires::FeaturePath pathToValue =
        babelwires::FeaturePath({babelwires::PathStep(babelwires::ValueElement::getStepToValue())});

    babelwires::FeaturePath pathToOptional = pathToValue;
    pathToOptional.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::getOpRecId()));

    babelwires::FeaturePath pathToIntInOptional = pathToOptional;
    pathToIntInOptional.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt1Id()));

    babelwires::FeaturePath pathToIntInSimpleRecord = pathToValue;
    pathToIntInSimpleRecord.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt1Id()));

    {
        babelwires::ActivateOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_pathToFeature = pathToValue;
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(testUtils::TestComplexRecordType::getOpRecId());
        testEnvironment.m_project.addModifier(elementId, activateOptionalsModifierData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = pathToOptional;
        inputConnection.m_pathToSourceFeature = pathToValue;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = pathToIntInSimpleRecord;
        outputConnection.m_pathToSourceFeature = pathToIntInOptional;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }

    const babelwires::RootFeature* const inputFeature = element->getInputFeature()->as<babelwires::RootFeature>();
    ASSERT_NE(inputFeature, nullptr);
    const babelwires::ValueFeature* const valueFeature = inputFeature->getFeature(0)->as<babelwires::ValueFeature>();
    const testUtils::TestComplexRecordType* const type = valueFeature->getType().as<testUtils::TestComplexRecordType>();

    babelwires::DeactivateOptionalCommand command("Test command", elementId, pathToValue,
                                                testUtils::TestComplexRecordType::getOpRecId());

    EXPECT_EQ(command.getName(), "Test command");

    const auto checkModifiers = [&testEnvironment, element, targetElement, pathToOptional, pathToIntInSimpleRecord](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(pathToOptional);
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(pathToIntInSimpleRecord);
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

    EXPECT_FALSE(type->isActivated(valueFeature->getValue(), testUtils::TestComplexRecordType::getOpRecId()));
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(type->isActivated(valueFeature->getValue(), testUtils::TestComplexRecordType::getOpRecId()));
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(type->isActivated(valueFeature->getValue(), testUtils::TestComplexRecordType::getOpRecId()));
    checkModifiers(true);
}
