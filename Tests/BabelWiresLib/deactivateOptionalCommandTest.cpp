#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(DeactivateOptionalsCommandTest, executeAndUndo) {
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

    const babelwires::FeaturePath pathToValue;

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

    const babelwires::ValueFeature* const valueFeature = element->getInputFeature()->as<babelwires::ValueFeature>();
    ASSERT_NE(valueFeature, nullptr);
    const testUtils::TestComplexRecordType* const type = valueFeature->getType().as<testUtils::TestComplexRecordType>();

    babelwires::DeactivateOptionalCommand command("Test command", elementId, pathToValue,
                                                  testUtils::TestComplexRecordType::getOpRecId());

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

TEST(DeactivateOptionalsCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51, babelwires::FeaturePath(), opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    babelwires::ValueElementData elementData(testUtils::TestComplexRecordType::getThisIdentifier());
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));

    const babelwires::FeaturePath pathToValue;

    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", elementId, pathToValue, opId);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));

    // Not an optional field
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);

    // Subrecord is a TestSimpleRecordType and has no optionals.
    const babelwires::FeaturePath pathToValue;

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, pathToValue, opId);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyAlreadyInactive) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));

    const babelwires::FeaturePath pathToValue;

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, pathToValue, testUtils::TestComplexRecordType::getOpIntId());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
