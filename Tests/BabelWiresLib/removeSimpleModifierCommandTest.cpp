#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/removeModifierCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

TEST(RemoveSimpleModifierCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData elementData;
    {
        babelwires::IntValueAssignmentData intAssignment;
        intAssignment.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_1;
        intAssignment.m_value = 12;
        elementData.m_modifiers.emplace_back(intAssignment.clone());
    }

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    testEnvironment.m_project.process();

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto checkModifiers = [&testEnvironment, element](bool isCommandExecuted) {
        const babelwires::Modifier* intAssignment =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray_1);
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
                                                    testUtils::TestRootFeature::s_pathToArray_1);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    checkModifiers(true);

    command.undo(testEnvironment.m_project);

    checkModifiers(false);

    command.execute(testEnvironment.m_project);

    checkModifiers(true);
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveSimpleModifierCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(RemoveSimpleModifierCommandTest, failSafelyNoModifier) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    babelwires::RemoveSimpleModifierCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"));

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
