#include <gtest/gtest.h>

#include "BabelWires/Commands/deactivateOptionalCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Features/recordWithOptionalsFeature.hpp"
#include "BabelWires/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWires/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testFeatureWithOptionals.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"

TEST(DeactivateOptionalsCommandTest, executeAndUndo) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());
    const babelwires::ElementId sourceId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const libTestUtils::TestFeatureElementWithOptionals* element =
        context.m_project.getFeatureElement(elementId)->asA<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        context.m_project.getFeatureElement(targetId)->asA<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->asA<libTestUtils::TestFeatureWithOptionals>();
    };

    ASSERT_NE(getInputFeature(), nullptr);

    {
        babelwires::ActivateOptionalsModifierData activateOptionalsModifierData;
        activateOptionalsModifierData.m_pathToFeature = libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord;
        activateOptionalsModifierData.m_selectedOptionals.emplace_back(getInputFeature()->m_op1Id);
        context.m_project.addModifier(elementId, activateOptionalsModifierData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = libTestUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1;
        inputConnection.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        context.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = libTestUtils::TestFeatureWithOptionals::s_pathToOp1_Int2;
        outputConnection.m_sourceId = elementId;
        context.m_project.addModifier(targetId, outputConnection);
    }

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, getInputFeature()->m_op1Id);

    const auto checkModifiers = [&context, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToOp1_Array_1);
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

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 3);
    checkModifiers(false);

    context.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(context.m_project));
    context.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
    // Note: We do not remove modifiers when a command happens to restore a value to default, so there's no test here to
    // check whether the modifier is present or not.

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_TRUE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 3);
    // Do check that the modifier is present after undo.
    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().asA<babelwires::ActivateOptionalsModifierData>(),
                  nullptr);
    }
    checkModifiers(false);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op0Id));
    EXPECT_FALSE(getInputFeature()->m_subrecord->isActivated(getInputFeature()->m_op1Id));
    EXPECT_EQ(getInputFeature()->m_subrecord->getNumFeatures(), 2);
    checkModifiers(true);
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::FieldIdentifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoRecord) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::FieldIdentifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", 51,
                                                  babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    libTestUtils::TestFeatureElementWithOptionalsData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyNoOptional) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element = context.m_project.getFeatureElement(elementId)->asA<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);

    babelwires::FieldIdentifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::DeactivateOptionalCommand command("Test command", elementId,
                                                  libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element = context.m_project.getFeatureElement(elementId)->asA<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->asA<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_ff0Id);

    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}

TEST(DeactivateOptionalsCommandTest, failSafelyAlreadyInactive) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element = context.m_project.getFeatureElement(elementId)->asA<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->asA<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::DeactivateOptionalCommand command(
        "Test command", elementId, libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op0Id);

    EXPECT_FALSE(command.initializeAndExecute(context.m_project));
}