#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/removeModifierCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

TEST(RemoveSimpleModifierCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
        intAssignment.m_value = 12;
        elementData.m_modifiers.emplace_back(intAssignment.clone());
    }

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    context.m_project.process();

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto checkModifiers = [&context, element](bool isCommandExecuted) {
        const babelwires::Modifier* intAssignment =
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_1);
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

    babelwires::RemoveSimpleModifierCommand command("Test command", elementId,
                                                    libTestUtils::TestRecordFeature::s_pathToArray_1);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initializeAndExecute(context.m_project));

    checkModifiers(true);

    command.undo(context.m_project);

    checkModifiers(false);

    command.execute(context.m_project);

    checkModifiers(true);
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveSimpleModifierCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoModifier) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveSimpleModifierCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}