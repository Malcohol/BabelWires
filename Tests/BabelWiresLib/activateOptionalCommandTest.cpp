#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/activateOptionalCommand.hpp>

#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureWithOptionals.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(ActivateOptionalsCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());
    const testUtils::TestFeatureElementWithOptionals* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::ActivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op1Id);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 3);
    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(testUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 2);
    EXPECT_EQ(element->getEdits().findModifier(testUtils::TestFeatureWithOptionals::s_pathToSubrecord), nullptr);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(inputFeature->m_subrecord->isActivated(inputFeature->m_op1Id));
    EXPECT_EQ(inputFeature->m_subrecord->getNumFeatures(), 3);
    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(testUtils::TestFeatureWithOptionals::s_pathToSubrecord);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }
}

TEST(ActivateOptionalsCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                                testUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                                babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    testUtils::TestFeatureElementWithOptionalsData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);

    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                                testUtils::TestFeatureWithOptionals::s_pathToSubrecord, opId);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::ActivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_ff0Id);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyAlreadyActivated) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithOptionalsData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithOptionals>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithOptionals* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithOptionals>();
    ASSERT_NE(inputFeature, nullptr);

    inputFeature->m_subrecord->activateField(inputFeature->m_op0Id);

    babelwires::ActivateOptionalCommand command(
        "Test command", elementId, testUtils::TestFeatureWithOptionals::s_pathToSubrecord, inputFeature->m_op0Id);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, executeAndUndoOnTypes) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));
    const babelwires::ValueElement* const element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);
    const babelwires::RootFeature* const inputFeature = element->getInputFeature()->as<babelwires::RootFeature>();
    ASSERT_NE(inputFeature, nullptr);

    const babelwires::ValueFeature* const valueFeature = inputFeature->getFeature(0)->as<babelwires::ValueFeature>();
    const testUtils::TestComplexRecordType* const type = valueFeature->getType().as<testUtils::TestComplexRecordType>();

    const babelwires::FeaturePath pathToValue =
        babelwires::FeaturePath({babelwires::PathStep(babelwires::ValueElement::getStepToValue())});

    babelwires::ActivateOptionalCommand command("Test command", elementId, pathToValue,
                                                testUtils::TestComplexRecordType::getOpRecId());

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(type->isActivated(valueFeature->getValue(), testUtils::TestComplexRecordType::getOpRecId()));

    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(pathToValue);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_FALSE(type->isActivated(valueFeature->getValue(), testUtils::TestComplexRecordType::getOpRecId()));
    EXPECT_EQ(element->getEdits().findModifier(pathToValue), nullptr);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_TRUE(type->isActivated(valueFeature->getValue(), testUtils::TestComplexRecordType::getOpRecId()));

    {
        const babelwires::Modifier* modifier =
            element->getEdits().findModifier(pathToValue);
        EXPECT_NE(modifier, nullptr);
        EXPECT_NE(modifier->getModifierData().as<babelwires::ActivateOptionalsModifierData>(), nullptr);
    }
}
