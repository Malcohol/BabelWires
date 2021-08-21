#include <gtest/gtest.h>

#include "BabelWires/Commands/removeModifierCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWires/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWires/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"
#include "Tests/BabelWires/TestUtils/testFeatureWithOptionals.hpp"

TEST(RemoveModifierCommandTest, executeAndUndoArray) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray;
        arrayInitialization.m_size = 5;
        elementData.m_modifiers.emplace_back(arrayInitialization.clone());
    }
    {
        // This is in the default size of the array, so should be unaffected.
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
        intAssignment.m_value = 12;
        elementData.m_modifiers.emplace_back(intAssignment.clone());
    }
    {
        // Failed.
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_3;
        inputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        inputConnection.m_sourceId = 57;
        elementData.m_modifiers.emplace_back(inputConnection.clone());
    }
    {
        babelwires::IntValueAssignmentData intAssignment2;
        intAssignment2.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_4;
        intAssignment2.m_value = 18;
        elementData.m_modifiers.emplace_back(intAssignment2.clone());
    }

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    context.m_project.process();

    const auto* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->asA<const libTestUtils::TestRecordFeature>();
    };
    const auto checkModifiers = [&context, element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayInitialization =
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray);
        const babelwires::Modifier* intAssignment =
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_1);
        const babelwires::Modifier* inputConnection =
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_3);
        const babelwires::Modifier* intAssignment2 =
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_4);
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
                                              libTestUtils::TestRecordFeature::s_pathToArray);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initializeAndExecute(context.m_project));

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(true);

    command.undo(context.m_project);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    command.execute(context.m_project);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(true);
}

TEST(RemoveModifierCommandTest, executeAndUndoOptionals)
{
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());
    const libTestUtils::TestFeatureElementWithOptionals* element =
        dynamic_cast<const libTestUtils::TestFeatureElementWithOptionals*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return dynamic_cast<const libTestUtils::TestFeatureWithOptionals*>(element->getInputFeature());
    };

    ASSERT_NE(getInputFeature(), nullptr);

    {
        babelwires::ActivateOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_pathToFeature = libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord;
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(getInputFeature()->m_op0Id);
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(getInputFeature()->m_op1Id);
        context.m_project.addModifier(elementId, activateOptionalsModifierData);
    }
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = libTestUtils::TestFeatureWithOptionals::s_pathToOp0;
        intAssignment.m_value = -19;
        context.m_project.addModifier(elementId, intAssignment);
    }
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = libTestUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1;
        intAssignment.m_value = 12;
        context.m_project.addModifier(elementId, intAssignment);
    }
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = libTestUtils::TestFeatureWithOptionals::s_pathToFf0;
        intAssignment.m_value = 100;
        context.m_project.addModifier(elementId, intAssignment);
    }

    const auto checkModifiers = [&context, element](bool isCommandExecuted) {
        const babelwires::Modifier* activateOptionals =
            element->findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        const babelwires::Modifier* intAssignment =
            element->findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToOp0);
        const babelwires::Modifier* intAssignment2 =
            element->findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1);
        const babelwires::Modifier* intAssignment3 =
            element->findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToFf0);
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
                                              libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 4);
    checkModifiers(false);

    EXPECT_TRUE(command.initializeAndExecute(context.m_project));

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);

    command.undo(context.m_project);

    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 4);
    checkModifiers(false);

    command.execute(context.m_project);

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
}

TEST(RemoveModifierCommandTest, failSafelyNoElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveModifierCommand command("Test command", 51,
                                              babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(RemoveModifierCommandTest, failSafelyNoModifier) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveModifierCommand command("Test command", 51,
                                              babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}
