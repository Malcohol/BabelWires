#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/Subcommands/removeSimpleModifierSubcommand.hpp>

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testRecordType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(RemoveSimpleModifierCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestComplexRecordElementData elementData;
    {
        babelwires::ValueAssignmentData intAssignment(babelwires::IntValue(12));
        intAssignment.m_targetPath = testDomain::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
        elementData.m_modifiers.emplace_back(intAssignment.clone());
    }

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(elementData);
    testEnvironment.m_project.process();

    const auto* element = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(element, nullptr);

    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* intAssignment = element->findModifier(testDomain::TestComplexRecordElementData::getPathToRecordArrayEntry(1));
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

    babelwires::RemoveSimpleModifierSubcommand testCopyConstructor(elementId, testDomain::TestComplexRecordElementData::getPathToRecordArrayEntry(1));
    babelwires::RemoveSimpleModifierSubcommand command = testCopyConstructor;

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    checkModifiers(true);

    command.undo(testEnvironment.m_project);

    checkModifiers(false);

    command.execute(testEnvironment.m_project);

    checkModifiers(true);
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveSimpleModifierSubcommand command(51, *babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testDomain::TestComplexRecordElementData());

    babelwires::RemoveSimpleModifierSubcommand command(elementId, *babelwires::Path::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
