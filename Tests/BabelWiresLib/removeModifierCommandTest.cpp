#include <gtest/gtest.h>

#include "BabelWiresLib/Project/Commands/removeModifierCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp"

TEST(RemoveModifierCommandTest, executeAndUndoArray) {
    babelwires::IdentifierRegistryScope identifierRegistry;
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
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_1;
        intAssignment.m_value = 12;
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
        babelwires::IntValueAssignmentData intAssignment2;
        intAssignment2.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_4;
        intAssignment2.m_value = 18;
        elementData.m_modifiers.emplace_back(intAssignment2.clone());
    }

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    testEnvironment.m_project.process();

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };
    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayInitialization =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray);
        const babelwires::Modifier* intAssignment =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray_1);
        const babelwires::Modifier* inputConnection =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray_3);
        const babelwires::Modifier* intAssignment2 =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray_4);
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

    babelwires::RemoveModifierCommand command("Test command", elementId,
                                              testUtils::TestRootFeature::s_pathToArray);

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

TEST(RemoveModifierCommandTest, executeAndUndoOptionals)
{
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());
    const testUtils::TestFeatureElementWithOptionals* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    };

    ASSERT_NE(getInputFeature(), nullptr);

    {
        babelwires::ActivateOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToSubrecord;
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(getInputFeature()->m_op0Id);
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(getInputFeature()->m_op1Id);
        testEnvironment.m_project.addModifier(elementId, activateOptionalsModifierData);
    }
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToOp0;
        intAssignment.m_value = -19;
        testEnvironment.m_project.addModifier(elementId, intAssignment);
    }
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1;
        intAssignment.m_value = 12;
        testEnvironment.m_project.addModifier(elementId, intAssignment);
    }
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToFf0;
        intAssignment.m_value = 100;
        testEnvironment.m_project.addModifier(elementId, intAssignment);
    }

    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* activateOptionals =
            element->findModifier(testUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        const babelwires::Modifier* intAssignment =
            element->findModifier(testUtils::TestFeatureWithOptionals::s_pathToOp0);
        const babelwires::Modifier* intAssignment2 =
            element->findModifier(testUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1);
        const babelwires::Modifier* intAssignment3 =
            element->findModifier(testUtils::TestFeatureWithOptionals::s_pathToFf0);
        int numModifiersAtElement = 0;
        for (const auto* m : element->getEdits().modifierRange()) {
            ++numModifiersAtElement;
        }
        if (isCommandExecuted) {
            EXPECT_EQ(activateOptionals, nullptr);
            EXPECT_EQ(intAssignment, nullptr);
            EXPECT_EQ(intAssignment2, nullptr);
            EXPECT_NE(intAssignment3, nullptr);
            EXPECT_EQ(numModifiersAtElement, 1);
        } else {
            EXPECT_NE(activateOptionals, nullptr);
            EXPECT_NE(intAssignment, nullptr);
            EXPECT_NE(intAssignment2, nullptr);
            EXPECT_NE(intAssignment3, nullptr);
            EXPECT_EQ(numModifiersAtElement, 4);
        }
    };

    babelwires::RemoveModifierCommand command("Test command", elementId,
                                              testUtils::TestFeatureWithOptionals::s_pathToSubrecord);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 4);
    checkModifiers(false);

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);

    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 4);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
}

TEST(RemoveModifierCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveModifierCommand command("Test command", 51,
                                              babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveModifierCommandTest, failSafelyNoModifier) {
    babelwires::IdentifierRegistryScope identifierRegistry;
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
