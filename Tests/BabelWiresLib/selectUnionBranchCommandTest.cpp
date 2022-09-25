#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/selectUnionBranchCommand.hpp>

#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/selectUnionBranchModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFeatureWithUnion.hpp>

TEST(SelectUnionBranchCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithUnionData());
    const babelwires::ElementId sourceId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId targetId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const testUtils::TestFeatureElementWithUnion* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithUnion>();
    ASSERT_NE(element, nullptr);
    const auto* targetElement =
        testEnvironment.m_project.getFeatureElement(targetId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(targetElement, nullptr);

    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<testUtils::TestFeatureWithUnion>();
    };
    ASSERT_NE(getInputFeature(), nullptr);

    {
        babelwires::SelectUnionBranchModifierData selectUnionBranchData;
        selectUnionBranchData.m_pathToFeature = testUtils::TestFeatureWithUnion::s_pathToUnionFeature;
        selectUnionBranchData.m_tagToSelect = getInputFeature()->m_tagBId;
        testEnvironment.m_project.addModifier(elementId, selectUnionBranchData);
    }
    {
        babelwires::ConnectionModifierData inputConnection;
        inputConnection.m_pathToFeature = testUtils::TestFeatureWithUnion::s_pathToFieldB0_Array_1;
        inputConnection.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        inputConnection.m_sourceId = sourceId;
        testEnvironment.m_project.addModifier(elementId, inputConnection);
    }
    {
        babelwires::ConnectionModifierData outputConnection;
        outputConnection.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        outputConnection.m_pathToSourceFeature = testUtils::TestFeatureWithUnion::s_pathToFieldB0_Int2;
        outputConnection.m_sourceId = elementId;
        testEnvironment.m_project.addModifier(targetId, outputConnection);
    }

    const auto checkModifiers = [&testEnvironment, element, targetElement](bool isCommandExecuted) {
        const babelwires::Modifier* inputConnection =
            element->findModifier(testUtils::TestFeatureWithUnion::s_pathToFieldB0_Array_1);
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
    
    babelwires::SelectUnionBranchCommand command("Test command", elementId,
                                               testUtils::TestFeatureWithUnion::s_pathToUnionFeature, getInputFeature()->m_tagAId);

    EXPECT_EQ(command.getName(), "Test command");
    EXPECT_EQ(getInputFeature()->m_unionFeature->getSelectedTag(), getInputFeature()->m_tagBId);
    checkModifiers(false);
    
    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(getInputFeature()->m_unionFeature->getSelectedTag(), getInputFeature()->m_tagAId);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_unionFeature->getSelectedTag(), getInputFeature()->m_tagBId);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_unionFeature->getSelectedTag(), getInputFeature()->m_tagAId);
    checkModifiers(true);
}

TEST(SelectUnionBranchCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectUnionBranchCommand command("Test command",  51,
                                               testUtils::TestFeatureWithUnion::s_pathToUnionFeature, "tag");

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectUnionBranchCommandTest, failSafelyNoRecord) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;
    babelwires::SelectUnionBranchCommand command("Test command",  51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), "tag");

    testUtils::TestFeatureElementWithUnionData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectUnionBranchCommandTest, failSafelyNoOptional) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithUnionData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithUnion>();
    ASSERT_NE(element, nullptr);

    babelwires::Identifier notATag("notTag");
    notATag.setDiscriminator(1);
    babelwires::SelectUnionBranchCommand command("Test command",  51,
                                               testUtils::TestFeatureWithUnion::s_pathToUnionFeature, notATag);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(SelectUnionBranchCommandTest, failSafelyAlreadySelected) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementWithUnionData());

    const auto* element = testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElementWithUnion>();
    ASSERT_NE(element, nullptr);
    const testUtils::TestFeatureWithUnion* inputFeature =
        element->getInputFeature()->as<testUtils::TestFeatureWithUnion>();
    ASSERT_NE(inputFeature, nullptr);

    babelwires::SelectUnionBranchCommand command("Test command", elementId,
                                               testUtils::TestFeatureWithUnion::s_pathToUnionFeature, inputFeature->m_tagBId);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

