#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/removeEntryFromArrayCommand.hpp"

#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

// TODO Test default array with non-minimum default size.

TEST(RemoveEntryFromArrayCommandTest, executeAndUndoNonDefaultArray) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId sourceId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray;
        arrayInitialization.m_size = 5;
        context.m_project.addModifier(elementId, arrayInitialization);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
        inputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        context.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
        outputConnection.m_sourceId = elementId;
        context.m_project.addModifier(targetId, outputConnection);
    }
    context.m_project.process();

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        context.m_project.getFeatureElement(targetId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const libTestUtils::TestRecordFeature>();
    };

    const auto checkModifiers = [&context, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_1);
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(libTestUtils::TestRecordFeature::s_pathToInt2);
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

    babelwires::RemoveEntryFromArrayCommand command("Test command", elementId,
                                                    libTestUtils::TestRecordFeature::s_pathToArray, 1, 1);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(context.m_project));

    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);
}

TEST(RemoveEntryFromArrayCommandTest, failSafelyNoElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveEntryFromArrayCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"), 1, 1);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(RemoveEntryFromArrayCommandTest, failSafelyNoArray) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::RemoveEntryFromArrayCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"), 1, 1);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(RemoveEntryFromArrayCommandTest, failSafelyOutOfRange) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto* inputFeature = element->getInputFeature()->as<const libTestUtils::TestRecordFeature>();
    ASSERT_NE(inputFeature, nullptr);
    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    babelwires::RemoveEntryFromArrayCommand command("Test command", elementId,
                                                    libTestUtils::TestRecordFeature::s_pathToArray, 3, 1);

    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}
