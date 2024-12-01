#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(RemoveSimpleModifierCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;
    {
        babelwires::ValueAssignmentData intAssignment(babelwires::IntValue(12));
        intAssignment.m_targetPath = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
        elementData.m_modifiers.emplace_back(intAssignment.clone());
    }

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(element, nullptr);

    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* intAssignment = element->findModifier(testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1));
        int numModifiersAtElement = 0;
        for (const auto* m : element->getEdits().modifierRange()) {
            ++numModifiersAtElement;
        }
        if (isCommandExecuted) {
            EXPECT_EQ(intAssignment, nullptr);
            EXPECT_EQ(numModifiersAtElement, 0);
        } else {
            EXPECT_NE(intAssignment, nullptr);
            EXPECT_EQ(numModifiersAtElement, 1);
        }
    };

    checkModifiers(false);

    babelwires::RemoveSimpleModifierSubcommand command(elementId, testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1));

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    checkModifiers(true);

    command.undo(testEnvironment.m_project);

    checkModifiers(false);

    command.execute(testEnvironment.m_project);

    checkModifiers(true);
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveSimpleModifierSubcommand command(51, babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    babelwires::RemoveSimpleModifierSubcommand command(elementId, babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
