#include <gtest/gtest.h>

#include "BabelWires/Commands/removeFailedModifiersCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWires/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"

namespace {
    void testRemoveFailedModifiers(bool isWholeRecord) {
        babelwires::FieldNameRegistryScope fieldNameRegistry;
        libTestUtils::TestProjectContext context;

        libTestUtils::TestFeatureElementData elementData;
        {
            // Will fail.
            babelwires::IntValueAssignmentData intAssignment;
            intAssignment.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_4;
            intAssignment.m_value = 12;
            elementData.m_modifiers.emplace_back(intAssignment.clone());
        }
        {
            // OK
            babelwires::ArraySizeModifierData arrayInitialization;
            arrayInitialization.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray;
            arrayInitialization.m_size = 3;
            elementData.m_modifiers.emplace_back(arrayInitialization.clone());
        }
        {
            // Will fail.
            babelwires::AssignFromFeatureData inputConnection;
            inputConnection.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
            inputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
            inputConnection.m_sourceId = 57;
            elementData.m_modifiers.emplace_back(inputConnection.clone());
        }

        const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
        context.m_project.process();

        const auto* element =
            dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
        ASSERT_NE(element, nullptr);

        const auto checkModifiers = [&context, element, isWholeRecord](bool isCommandExecuted) {
            const babelwires::Modifier* intAssignment =
                element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_4);
            const babelwires::Modifier* arrayInitialization =
                element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray);
            const babelwires::Modifier* inputConnection =
                element->findModifier(libTestUtils::TestRecordFeature::s_pathToInt2);
            int numModifiersAtElement = 0;
            int numModifiersAtTarget = 0;
            for (const auto* m : element->getEdits().modifierRange()) {
                ++numModifiersAtElement;
            }
            if (isCommandExecuted) {
                EXPECT_EQ(intAssignment, nullptr);
                EXPECT_NE(arrayInitialization, nullptr);
                if (isWholeRecord) {
                    EXPECT_EQ(inputConnection, nullptr);
                    EXPECT_EQ(numModifiersAtElement, 1);
                } else {
                    EXPECT_NE(inputConnection, nullptr);
                    EXPECT_EQ(numModifiersAtElement, 2);
                }
            } else {
                EXPECT_NE(intAssignment, nullptr);
                EXPECT_TRUE(intAssignment->isFailed());
                EXPECT_NE(arrayInitialization, nullptr);
                EXPECT_FALSE(arrayInitialization->isFailed());
                EXPECT_NE(inputConnection, nullptr);
                EXPECT_TRUE(inputConnection->isFailed());
                EXPECT_EQ(numModifiersAtElement, 3);
            }
        };

        checkModifiers(false);

        const babelwires::FeaturePath commandPath =
            isWholeRecord ? babelwires::FeaturePath() : libTestUtils::TestRecordFeature::s_pathToArray;
        babelwires::RemoveFailedModifiersCommand command("Test command", elementId, commandPath);

        EXPECT_EQ(command.getName(), "Test command");

        EXPECT_TRUE(command.initializeAndExecute(context.m_project));
        context.m_project.process();

        checkModifiers(true);

        command.undo(context.m_project);
        context.m_project.process();

        checkModifiers(false);

        command.execute(context.m_project);
        context.m_project.process();

        checkModifiers(true);
    }
} // namespace

TEST(RemoveFailedModifiersCommandTest, executeAndUndoWholeRecord) {
    testRemoveFailedModifiers(true);
}

TEST(RemoveFailedModifiersCommandTest, executeAndUndoSubFeature) {
    testRemoveFailedModifiers(false);
}

TEST(RemoveFailedModifiersCommandTest, failSafelyNoElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveFailedModifiersCommand command("Test command", 51,
                                                     babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(RemoveFailedModifiersCommandTest, failSafelyNoSubFeature) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveFailedModifiersCommand command("Test command", 51,
                                                     babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}
