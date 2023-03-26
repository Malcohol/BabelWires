#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/setArraySizeCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

// TODO Test default array with non-minimum default size.

TEST(SetArraySizeCommandTest, executeAndUndoArrayGrow) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray;
        arrayInitialization.m_size = 3;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_1;
        inputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToArray_2;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };

    const auto checkModifiers = [&testEnvironment, element, targetElement]() {
        const babelwires::Modifier* inputConnection =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray_1);
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
        EXPECT_NE(inputConnection, nullptr);
        EXPECT_NE(outputConnection, nullptr);
        EXPECT_EQ(numModifiersAtElement, 2);
        EXPECT_EQ(numModifiersAtTarget, 1);
    };

    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers();

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    testUtils::TestRootFeature::s_pathToArray, 5);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers();

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers();

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers();
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayShrink) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray;
        arrayInitialization.m_size = 5;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_3;
        inputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToArray_4;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    testEnvironment.m_project.process();

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };

    const auto checkModifiers = [&testEnvironment, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray_3);
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

    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    testUtils::TestRootFeature::s_pathToArray, 3);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayNoPriorModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    testEnvironment.m_project.process();

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };

    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray);
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    testUtils::TestRootFeature::s_pathToArray, 4);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 4);
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
    babelwires::SetArraySizeCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"), 4);

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SetArraySizeCommandTest, failSafelyOutOfRange) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto* inputFeature = element->getInputFeature()->as<const testUtils::TestRootFeature>();
    ASSERT_NE(inputFeature, nullptr);
    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    testUtils::TestRootFeature::s_pathToArray, 12);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
