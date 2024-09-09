#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/setArraySizeCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

// TODO Test default array with non-minimum default size.

TEST(SetArraySizeCommandTest, executeAndUndoArrayGrow) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleArrayType::getThisIdentifier()));
    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleRecordType::getThisIdentifier()));
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleRecordType::getThisIdentifier()));

    babelwires::FeaturePath pathToArray;
    pathToArray.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    babelwires::FeaturePath pathToArray_1 = pathToArray;
    pathToArray_1.pushStep(babelwires::PathStep(1));
    babelwires::FeaturePath pathToArray_2 = pathToArray;
    pathToArray_2.pushStep(babelwires::PathStep(1));
    babelwires::FeaturePath pathToRecordInt;
    pathToRecordInt.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    pathToRecordInt.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()));

    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = pathToArray;
        arrayInitialization.m_size = 3;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = pathToArray_1;
        inputConnection.m_pathToSourceFeature = pathToRecordInt;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = pathToRecordInt;
        outputConnection.m_pathToSourceFeature = pathToArray_2;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);
    const auto* targetElement = testEnvironment.m_project.getFeatureElement(targetId);
    ASSERT_NE(targetElement, nullptr);

    const auto getArrayFeature = [element]() {
        auto root = element->getInputFeature()->as<babelwires::CompoundFeature>();
        return root->getFeature(0)->as<babelwires::ValueFeature>();
    };

    const auto checkModifiers = [&testEnvironment, element, targetElement, pathToArray_1, pathToRecordInt]() {
        const babelwires::Modifier* inputConnection = element->findModifier(pathToArray_1);
        const babelwires::Modifier* outputConnection = targetElement->findModifier(pathToRecordInt);
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

    ASSERT_NE(getArrayFeature(), nullptr);

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 3);
    checkModifiers();

    babelwires::SetArraySizeCommand command("Test command", elementId, pathToArray, 5);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 5);
    checkModifiers();

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 3);
    checkModifiers();

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 5);
    checkModifiers();
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayShrink) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleArrayType::getThisIdentifier()));
    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleRecordType::getThisIdentifier()));
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleRecordType::getThisIdentifier()));

    babelwires::FeaturePath pathToArray;
    pathToArray.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    babelwires::FeaturePath pathToArray_1 = pathToArray;
    pathToArray_1.pushStep(babelwires::PathStep(1));
    babelwires::FeaturePath pathToArray_2 = pathToArray;
    pathToArray_2.pushStep(babelwires::PathStep(1));
    babelwires::FeaturePath pathToRecordInt;
    pathToRecordInt.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));
    pathToRecordInt.pushStep(babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id()));

    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = pathToArray;
        arrayInitialization.m_size = 3;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = pathToArray_1;
        inputConnection.m_pathToSourceFeature = pathToRecordInt;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = pathToRecordInt;
        outputConnection.m_pathToSourceFeature = pathToArray_2;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);
    const auto* targetElement = testEnvironment.m_project.getFeatureElement(targetId);
    ASSERT_NE(targetElement, nullptr);

    const auto getArrayFeature = [element]() {
        auto root = element->getInputFeature()->as<babelwires::CompoundFeature>();
        return root->getFeature(0)->as<babelwires::ValueFeature>();
    };

    const auto checkModifiers = [&testEnvironment, element, targetElement, pathToArray_1,
                                 pathToRecordInt](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection = element->findModifier(pathToArray_1);
        const babelwires::Modifier* outputConnection = targetElement->findModifier(pathToRecordInt);
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

    ASSERT_NE(getArrayFeature(), nullptr);

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 3);
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId, pathToArray, 1);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 1);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 3);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), 1);
    checkModifiers(true);
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayNoPriorModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleArrayType::getThisIdentifier()));
    testEnvironment.m_project.process();

    babelwires::FeaturePath pathToArray;
    pathToArray.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    const auto getArrayFeature = [element]() {
        auto root = element->getInputFeature()->as<babelwires::CompoundFeature>();
        return root->getFeature(0)->as<babelwires::ValueFeature>();
    };

    const auto checkModifiers = [&testEnvironment, element, pathToArray](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier = element->findModifier(pathToArray);
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    ASSERT_NE(getArrayFeature(), nullptr);

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId, pathToArray,
                                            testUtils::TestSimpleArrayType::s_nonDefaultSize);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_nonDefaultSize);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_nonDefaultSize);
    checkModifiers(true);
}

TEST(SetArraySizeCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SetArraySizeCommand command("Test command", 51,
                                            babelwires::FeaturePath::deserializeFromString("qqq/zzz"), 4);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SetArraySizeCommandTest, failSafelyNoArray) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleRecordType::getThisIdentifier()));

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                            babelwires::FeaturePath::deserializeFromString("qqq/zzz"), 4);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SetArraySizeCommandTest, failSafelyOutOfRange) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestSimpleArrayType::getThisIdentifier()));

    babelwires::FeaturePath pathToArray;
    pathToArray.pushStep(babelwires::PathStep(babelwires::ValueElement::getStepToValue()));

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    const auto getArrayFeature = [element]() {
        auto root = element->getInputFeature()->as<babelwires::CompoundFeature>();
        return root->getFeature(0)->as<babelwires::ValueFeature>();
    };

    ASSERT_NE(getArrayFeature(), nullptr);
    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);

    babelwires::SetArraySizeCommand command("Test command", elementId, pathToArray, 12);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
}
