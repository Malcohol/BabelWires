#include <gtest/gtest.h>

#include "BabelWiresLib/Project/Commands/deactivateOptionalCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Features/recordWithOptionalsFeature.hpp"
#include "BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"

TEST(DeactivateOptionalsCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());
    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const testUtils::TestFeatureElementWithOptionals* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    };

    ASSERT_NE(getInputFeature(), nullptr);

    {
        babelwires::ActivateOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToSubrecord;
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(getInputFeature()->m_op1Id);
        testEnvironment.m_project.addModifier(elementId, activateOptionalsModifierData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = testUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1;
        inputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = testUtils::TestFeatureWithOptionals::s_pathToOp1_Int2;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, getInputFeature()->m_op1Id);

    const auto checkModifiers = [&testEnvironment, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(testUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1);
        const babelwires::Modifier* outputConnection =
            targetElement->findModifier(testUtils::TestRootFeature::s_pathToInt2);
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

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 3);
    checkModifiers(false);

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));
    testEnvironment.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
    // Note: We do not remove modifiers when a command happens to restore a value to default, so there's no test here to
    // check whether the modifier is present or not.

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 3);
    // Do check that the modifier is present after undo.
    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(testUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(),
                  nullptr);
    }
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    babelwires::Identifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  testUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoRecord) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    babelwires::Identifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    testUtils::TestFeatureElementWithOptionalsData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoOptional) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);

    babelwires::Identifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", elementId,
                                                  testUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_ff0Id);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyAlreadyInactive) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op0Id);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}