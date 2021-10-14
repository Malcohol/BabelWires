#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/activateOptionalCommand.hpp"

#include "BabelWiresLib/Features/recordWithOptionalsFeature.hpp"
#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp"

TEST(ActivateOptionalsCommandTest, executeAndUndo) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());
    const libTestUtils::TestFeatureElementWithOptionals* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op1Id);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_TRUE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 3);
    {
        const babelwires::Modifier* modifier = element->getEdits().findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_FALSE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 2);
    EXPECT_EQ(element->getEdits().findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord), nullptr);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_TRUE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 3);
    {
        const babelwires::Modifier* modifier = element->getEdits().findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }
}


TEST(ActivateOptionalsCommandTest, failSafelyNoElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::FieldIdentifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoRecord) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::FieldIdentifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    libTestUtils::TestFeatureElementWithOptionalsData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoOptional) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);

    babelwires::FieldIdentifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_ff0Id);

    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyAlreadyActivated) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    inputFeature->m_subrecord->activateField(inputFeature->m_op0Id);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op0Id);

    EXPECT_FALSE(command.initialize(context.m_project));
}