#include <gtest/gtest.h>

#include "BabelWires/Commands/removeModifierCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"

TEST(RemoveModifierCommandTest, executeAndUndo) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    {
        babelwires::ArrayInitializationData arrayInitialization;
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
        babelwires::AssignFromFeatureData inputConnection;
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
        return dynamic_cast<const libTestUtils::TestRecordFeature*>(element->getInputFeature());
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
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(true);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
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
