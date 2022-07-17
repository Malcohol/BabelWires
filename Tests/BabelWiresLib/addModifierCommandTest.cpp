#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addModifierCommand.hpp>

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

TEST(AddModifierCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);
    EXPECT_NE(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_EQ(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", elementId, modData.clone());

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    testEnvironment.m_project.process();

    command.execute(testEnvironment.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);

    command.undo(testEnvironment.m_project);
    EXPECT_NE(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_EQ(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);

    command.execute(testEnvironment.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);
}

TEST(AddModifierCommandTest, executeAndUndoPreexistingModifier) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData elementData;
    {
        babelwires::IntValueAssignmentData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_value = 77;
        elementData.m_modifiers.emplace_back(modData.clone());
    }

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 77);
    EXPECT_NE(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", elementId, modData.clone());

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    testEnvironment.m_project.process();

    command.execute(testEnvironment.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);

    command.undo(testEnvironment.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 77);
    EXPECT_NE(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);

    command.execute(testEnvironment.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(testUtils::TestRootFeature::s_pathToInt2), nullptr);
}

TEST(AddModifierCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", 51, modData.clone());

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(AddModifierCommandTest, failSafelyNoTarget) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", 51, modData.clone());

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}
