#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/removeModifierCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordWithVariantsType.hpp>

TEST(RemoveModifierCommandTest, executeAndUndoArray) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestArrayElementData());
    const babelwires::ElementId sourceId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestSimpleRecordElementData());
    const babelwires::ElementId targetId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestSimpleRecordElementData());

    const unsigned int initialArraySize = testUtils::TestSimpleArrayType::s_defaultSize + 2;
    const babelwires::Path pathToArrayEntry =
        testUtils::TestArrayElementData::getPathToArrayEntry(testUtils::TestSimpleArrayType::s_defaultSize + 1);

    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = testUtils::TestArrayElementData::getPathToArray();
        arrayInitialization.m_size = initialArraySize;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = pathToArrayEntry;
        inputConnection.m_pathToSourceFeature = testUtils::TestSimpleRecordElementData::getPathToRecordInt0();
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = testUtils::TestSimpleRecordElementData::getPathToRecordInt0();
        outputConnection.m_pathToSourceFeature = pathToArrayEntry;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);
    const auto* targetElement = testEnvironment.m_project.getFeatureElement(targetId);
    ASSERT_NE(targetElement, nullptr);

    const auto checkModifiers = [&testEnvironment, element, targetElement, pathToArrayEntry](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(pathToArrayEntry);
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
            EXPECT_EQ(numModifiersAtElement, 0);
            EXPECT_EQ(numModifiersAtTarget, 0);
        } else {
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 2);
            EXPECT_EQ(numModifiersAtTarget, 1);
        }
    };

    ASSERT_NE(element->getInputFeature(), nullptr);

    EXPECT_EQ(element->getInputFeature()->getNumFeatures(), initialArraySize);

    babelwires::RemoveModifierCommand command("Test command", elementId,
                                              testUtils::TestArrayElementData::getPathToArray());

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInputFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInputFeature()->getNumFeatures(), initialArraySize);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(element->getInputFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(true);
}

TEST(RemoveModifierCommandTest, executeAndUndoOptionals) {
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

    const babelwires::Path pathToValue;

    babelwires::Path pathToOptional = pathToValue;
    pathToOptional.pushStep(babelwires::PathStep(testUtils::TestComplexRecordType::getOpRecId()));

    babelwires::Path pathToIntInOptional = pathToOptional;
    pathToIntInOptional.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt1Id()));

    babelwires::Path pathToIntInSimpleRecord = pathToValue;
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

    const babelwires::ValueTreeNode* const valueFeature = element->getInputFeature();
    ASSERT_NE(valueFeature, nullptr);
    const testUtils::TestComplexRecordType* const type = valueFeature->getType().as<testUtils::TestComplexRecordType>();

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

TEST(RemoveModifierCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveModifierCommand command("Test command", 51,
                                              babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveModifierCommandTest, failSafelyNoModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestSimpleRecordElementData());

    babelwires::RemoveModifierCommand command("Test command", elementId,
                                              babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
