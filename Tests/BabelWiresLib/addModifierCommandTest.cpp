#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/addModifierCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

TEST(AddModifierCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const libTestUtils::TestRecordFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);
    EXPECT_NE(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_EQ(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", elementId, modData.clone());

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));
    context.m_project.process();

    command.execute(context.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);

    command.undo(context.m_project);
    EXPECT_NE(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_EQ(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);

    command.execute(context.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);
}

TEST(AddModifierCommandTest, executeAndUndoPreexistingModifier) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    {
        babelwires::IntValueAssignmentData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_value = 77;
        elementData.m_modifiers.emplace_back(modData.clone());
    }

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const libTestUtils::TestRecordFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 77);
    EXPECT_NE(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", elementId, modData.clone());

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));
    context.m_project.process();

    command.execute(context.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);

    command.undo(context.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 77);
    EXPECT_NE(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);

    command.execute(context.m_project);
    EXPECT_EQ(getInputFeature()->m_intFeature2->get(), 86);
    EXPECT_NE(element->getEdits().findModifier(libTestUtils::TestRecordFeature::s_pathToInt2), nullptr);
}

TEST(AddModifierCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", 51, modData.clone());

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(AddModifierCommandTest, failSafelyNoTarget) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", 51, modData.clone());

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}
