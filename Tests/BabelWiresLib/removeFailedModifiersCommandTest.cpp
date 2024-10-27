#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/removeFailedModifiersCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

namespace {
    void testRemoveFailedModifiers(bool isWholeRecord) {
        testUtils::TestEnvironment testEnvironment;

        testUtils::TestComplexRecordElementData elementData;
        {
            // Will fail.
            babelwires::ValueAssignmentData intAssignment(babelwires::IntValue(12));
            intAssignment.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(
                testUtils::TestSimpleArrayType::s_maximumSize);
            elementData.m_modifiers.emplace_back(intAssignment.clone());
        }
        {
            // OK
            babelwires::ArraySizeModifierData arrayInitialization;
            arrayInitialization.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordArray();
            arrayInitialization.m_size = testUtils::TestSimpleArrayType::s_nonDefaultSize;
            elementData.m_modifiers.emplace_back(arrayInitialization.clone());
        }
        {
            // Will fail.
            babelwires::ConnectionModifierData inputConnection;
            inputConnection.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordInt1();
            inputConnection.m_sourcePath = testUtils::TestComplexRecordElementData::getPathToRecordInt1();
            inputConnection.m_sourceId = 57;
            elementData.m_modifiers.emplace_back(inputConnection.clone());
        }

        const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
        testEnvironment.m_project.process();

        const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
        ASSERT_NE(element, nullptr);

        const auto checkModifiers = [&testEnvironment, element, isWholeRecord](bool isCommandExecuted) {
            const babelwires::Modifier* intAssignment =
                element->findModifier(testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(
                    testUtils::TestSimpleArrayType::s_maximumSize));
            const babelwires::Modifier* arrayInitialization =
                element->findModifier(testUtils::TestComplexRecordElementData::getPathToRecordArray());
            const babelwires::Modifier* inputConnection =
                element->findModifier(testUtils::TestComplexRecordElementData::getPathToRecordInt1());
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

        const babelwires::Path commandPath =
            isWholeRecord ? babelwires::Path() : testUtils::TestComplexRecordElementData::getPathToRecordArray();
        babelwires::RemoveFailedModifiersCommand command("Test command", elementId, commandPath);

        EXPECT_EQ(command.getName(), "Test command");

        EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
        testEnvironment.m_project.process();

        checkModifiers(true);

        command.undo(testEnvironment.m_project);
        testEnvironment.m_project.process();

        checkModifiers(false);

        command.execute(testEnvironment.m_project);
        testEnvironment.m_project.process();

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
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveFailedModifiersCommand command("Test command", 51,
                                                     babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveFailedModifiersCommandTest, failSafelyNoSubFeature) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestComplexRecordElementData());

    babelwires::RemoveFailedModifiersCommand command("Test command", elementId,
                                                     babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
