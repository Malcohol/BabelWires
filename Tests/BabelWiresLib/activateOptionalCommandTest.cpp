#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/activateOptionalCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/activateOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(ActivateOptionalsCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));
    const babelwires::ValueElement* const element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);

    const babelwires::ValueFeature* const valueFeature = element->getInputFeature()->as<babelwires::ValueFeature>();
    ASSERT_NE(valueFeature, nullptr);
    const testUtils::TestComplexRecordType* const type = valueFeature->getType().as<testUtils::TestComplexRecordType>();

    const babelwires::FeaturePath pathToValue;

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

TEST(ActivateOptionalsCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                                babelwires::FeaturePath(), opId);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);
    babelwires::ActivateOptionalCommand command("Test command", 51,
                                                babelwires::FeaturePath::deserializeFromString("qqq/zzz"), opId);

    babelwires::ValueElementData elementData(testUtils::TestComplexRecordType::getThisIdentifier());
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyNoOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));

    const babelwires::FeaturePath pathToValue;

    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);

    babelwires::ActivateOptionalCommand command("Test command", elementId,
                                                pathToValue, opId);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyFieldNotOptional) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));

    // Not an optional field
    babelwires::ShortId opId("flerm");
    opId.setDiscriminator(1);

    // Subrecord is a TestSimpleRecordType and has no optionals.
    const babelwires::FeaturePath pathToValue;

    babelwires::ActivateOptionalCommand command(
        "Test command", elementId, pathToValue, opId);

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ActivateOptionalsCommandTest, failSafelyAlreadyActivated) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(
        babelwires::ValueElementData(testUtils::TestComplexRecordType::getThisIdentifier()));
    babelwires::ValueElement* const element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);

    const babelwires::FeaturePath pathToValue;

    auto* const inputRecord = element->getInputFeatureNonConst(pathToValue)->as<babelwires::ValueFeature>();
    ASSERT_NE(inputRecord, nullptr);

    // Active the optional first.
    testUtils::TestComplexRecordType::Instance instance{*inputRecord};
    instance.activateAndGetopInt();

    babelwires::ActivateOptionalCommand command(
        "Test command", elementId, pathToValue, testUtils::TestComplexRecordType::getOpIntId());

    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
