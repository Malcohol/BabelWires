#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/setArraySizeCommand.hpp"

#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

// TODO Test default array with non-minimum default size.

TEST(SetArraySizeCommandTest, executeAndUndoArrayGrow) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId sourceId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray;
        arrayInitialization.m_size = 3;
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
        outputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToArray_2;
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

    const auto checkModifiers = [&context, element, targetElement]() {
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
        EXPECT_NE(inputConnection, nullptr);
        EXPECT_NE(outputConnection, nullptr);
        EXPECT_EQ(numModifiersAtElement, 2);
        EXPECT_EQ(numModifiersAtTarget, 1);
    };

    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers();

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    libTestUtils::TestRecordFeature::s_pathToArray, 5);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(context.m_project));

    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers();

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers();

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers();
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayShrink) {
    babelwires::IdentifierRegistryScope identifierRegistry;
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
        inputConnection.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_3;
        inputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        context.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToArray_4;
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
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_3);
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
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    libTestUtils::TestRecordFeature::s_pathToArray, 3);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(context.m_project));

    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 5);
    checkModifiers(false);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);
}

TEST(SetArraySizeCommandTest, executeAndUndoArrayNoPriorModifier) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    context.m_project.process();

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const libTestUtils::TestRecordFeature>();
    };

    const auto checkModifiers = [&context, element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray);
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    libTestUtils::TestRecordFeature::s_pathToArray, 4);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(context.m_project));

    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);
}

TEST(SetArraySizeCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::SetArraySizeCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"), 4);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(SetArraySizeCommandTest, failSafelyNoArray) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::SetArraySizeCommand command("Test command", 51,
                                                    babelwires::FeaturePath::deserializeFromString("qqq/zzz"), 4);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(SetArraySizeCommandTest, failSafelyOutOfRange) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto* inputFeature = element->getInputFeature()->as<const libTestUtils::TestRecordFeature>();
    ASSERT_NE(inputFeature, nullptr);
    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    babelwires::SetArraySizeCommand command("Test command", elementId,
                                                    libTestUtils::TestRecordFeature::s_pathToArray, 12);

    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}
