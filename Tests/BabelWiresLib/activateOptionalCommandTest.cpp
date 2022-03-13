#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/activateOptionalCommand.hpp"

#include "BabelWiresLib/Features/recordWithOptionalsFeature.hpp"
#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp"

TEST(ActivateOptionalsCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());
    const libTestUtils::TestFeatureElementWithOptionals* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op1Id);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 3);
    {
        const babelwires::Modifier* modifier = element->getEdits().findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 2);
    EXPECT_EQ(element->getEdits().findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord), nullptr);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 3);
    {
        const babelwires::Modifier* modifier = element->getEdits().findModifier(libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }
}


TEST(ActivateOptionalsCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;
    babelwires::Identifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoRecord) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;
    babelwires::Identifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    libTestUtils::TestFeatureElementWithOptionalsData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoOptional) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);

    babelwires::Identifier opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_ff0Id);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyAlreadyActivated) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(libTestUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<libTestUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    inputFeature->m_subrecord->activateField(inputFeature->m_op0Id);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                               libTestUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op0Id);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}