#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/selectRecordVariantCommand.hpp>

#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordWithVariantsType.hpp>

namespace {
    struct TestElementWithVariantsData : babelwires::ValueElementData {
        TestElementWithVariantsData()
            : ValueElementData(testUtils::TestRecordWithVariantsType::getThisIdentifier()) {}
    };

    static babelwires::FeaturePath getPathToRecordWithVariants() {
        return std::vector<babelwires::PathStep>{babelwires::PathStep(babelwires::ValueElement::getStepToValue())};
    }

    static const babelwires::FeaturePath getPathToFieldA0() {
        return std::vector<babelwires::PathStep>{babelwires::PathStep(babelwires::ValueElement::getStepToValue()),
                                                    babelwires::PathStep(testUtils::TestRecordWithVariantsType::getFieldA0Id())};
    }

    static const babelwires::FeaturePath getPathToFieldA1_Int0() {
        return std::vector<babelwires::PathStep>{babelwires::PathStep(babelwires::ValueElement::getStepToValue()),
                                                    babelwires::PathStep(testUtils::TestRecordWithVariantsType::getFieldA1Id()),
                                                    babelwires::PathStep(testUtils::TestSimpleRecordType::getInt0Id())};
    }

    static const babelwires::FeaturePath getPathToFieldAB() {
        return std::vector<babelwires::PathStep>{babelwires::PathStep(babelwires::ValueElement::getStepToValue()),
                                                    babelwires::PathStep(testUtils::TestRecordWithVariantsType::getFieldABId())};
    }

} // namespace testUtils

TEST(SelectRecordVariantCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(TestElementWithVariantsData());
    const babelwires::ElementId sourceId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId targetId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const babelwires::ValueElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(targetElement, nullptr);

    const auto getInputValueFeature = [element]() {
        const auto* root = element->getInputFeature()->as<babelwires::RootFeature>();
        return root->getFeature(0)->as<babelwires::ValueFeature>();
    };
    const auto getSelectedTag = [](const babelwires::ValueFeature* valueFeature) {
        const auto& type = valueFeature->getType().is<testUtils::TestRecordWithVariantsType>();
        return type.getSelectedTag(valueFeature->getValue());
    };

    ASSERT_NE(getInputValueFeature(), nullptr);

    {
        babelwires::SelectRecordVariantModifierData selectRecordVariantData;
        selectRecordVariantData.m_pathToFeature = getPathToRecordWithVariants();
        selectRecordVariantData.m_tagToSelect = testUtils::TestRecordWithVariantsType::getTagAId();
        testEnvironment.m_project.addModifier(elementId, selectRecordVariantData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = getPathToFieldA1_Int0();
        inputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = getPathToFieldA0();
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }
    {
        babelwires::ValueAssignmentData assignInt(babelwires::IntValue(12));
        assignInt.m_pathToFeature = getPathToFieldAB();
        testEnvironment.m_project.addModifier(elementId, assignInt);
    }
    {
        babelwires::ValueAssignmentData assignInt(babelwires::IntValue(4));
        assignInt.m_pathToFeature = getPathToFieldA0();
        testEnvironment.m_project.addModifier(elementId, assignInt);
    }

    const auto checkModifiers = [&testEnvironment, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(getPathToFieldA1_Int0());
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
            EXPECT_EQ(numModifiersAtElement, 2);
            EXPECT_EQ(numModifiersAtTarget, 0);
        } else {
            EXPECT_NE(inputConnection, nullptr);
            EXPECT_NE(outputConnection, nullptr);
            EXPECT_EQ(numModifiersAtElement, 4);
            EXPECT_EQ(numModifiersAtTarget, 1);
        }
    };

    babelwires::SelectRecordVariantCommand command("Test command", elementId,
                                                 getPathToRecordWithVariants(),
                                                 testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_EQ(getSelectedTag(getInputValueFeature()), testUtils::TestRecordWithVariantsType::getTagAId());
    checkModifiers(false);

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(getSelectedTag(getInputValueFeature()), testUtils::TestRecordWithVariantsType::getTagBId());
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getSelectedTag(getInputValueFeature()), testUtils::TestRecordWithVariantsType::getTagAId());
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getSelectedTag(getInputValueFeature()), testUtils::TestRecordWithVariantsType::getTagBId());
    checkModifiers(true);
}

TEST(SelectRecordVariantCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectRecordVariantCommand command("Test command", 51,
                                                 getPathToRecordWithVariants(), "tag");

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyNoRecord) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectRecordVariantCommand command("Test command", 51,
                                                 babelwires::FeaturePath::deserializeFromString("qqq/zzz"), "tag");

    TestElementWithVariantsData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyNotATag) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(TestElementWithVariantsData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);

    babelwires::ShortId notATag("notTag");
    notATag.setDiscriminator(1);
    babelwires::SelectRecordVariantCommand command("Test command", 51,
                                                 getPathToRecordWithVariants(), notATag);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectRecordVariantCommandTest, failSafelyAlreadySelected) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(TestElementWithVariantsData());

    babelwires::SelectRecordVariantCommand command(
        "Test command", elementId, getPathToRecordWithVariants(), testUtils::TestRecordWithVariantsType::getTagBId());

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}
