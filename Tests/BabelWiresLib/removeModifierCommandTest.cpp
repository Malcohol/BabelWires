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

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(RemoveModifierCommandTest, executeAndUndoArray) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData elementData;
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray;
        arrayInitialization.m_size = 5;
        elementData.m_modifiers.emplace_back(arrayInitialization.clone());
    }
    {
        // This is in the default size of the array, so should be unaffected.
        babelwires::ValueAssignmentData intAssignment(babelwires::IntValue(12));
        intAssignment.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_1;
        elementData.m_modifiers.emplace_back(intAssignment.clone());
    }
    {
        // Failed.
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_3;
        inputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        inputConnection.m_sourceId = 57;
        elementData.m_modifiers.emplace_back(inputConnection.clone());
    }
    {
        babelwires::ValueAssignmentData intAssignment2(babelwires::IntValue(18));
        intAssignment2.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_4;
        elementData.m_modifiers.emplace_back(intAssignment2.clone());
    }

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };
    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayInitialization =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray);
        const babelwires::Modifier* intAssignment = element->findModifier(testUtils::TestRootFeature::s_pathToArray_1);
        const babelwires::Modifier* inputConnection =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray_3);
        const babelwires::Modifier* intAssignment2 = element->findModifier(testUtils::TestRootFeature::s_pathToArray_4);
        int numModifiersAtElement = 0;
        for (const auto* m : element->getEdits().modifierRange()) {
            ++numModifiersAtElement;
        }
        if (isCommandExecuted) {
            EXPECT_EQ(arrayInitialization, nullptr);
            EXPECT_NE(intAssignment, nullptr);
            EXPECT_EQ(inputConnection, nullptr);
            EXPECT_EQ(intAssignment2, nullptr);
            EXPECT_EQ(numModifiersAtElement, 1);
        } else {
            EXPECT_NE(arrayInitialization, nullptr);
            EXPECT_NE(intAssignment, nullptr);
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(intAssignment2, nullptr);
            EXPECT_EQ(numModifiersAtElement, 4);
        }
    };

    ASSERT_NE(getInputFeature(), nullptr);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    babelwires::RemoveModifierCommand command("Test command", elementId, testUtils::TestRootFeature::s_pathToArray);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
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
                                              babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveModifierCommandTest, failSafelyNoModifier) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveModifierCommand command("Test command", 51,
                                              babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
