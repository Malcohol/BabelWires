#include <gtest/gtest.h>

#include "BabelWires/Commands/addModifierCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"

TEST(AddModifierCommandTest, executeAndUndo) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return dynamic_cast<const libTestUtils::TestRecordFeature*>(element->getInputFeature());
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
    babelwires::FieldNameRegistryScope fieldNameRegistry;
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
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return dynamic_cast<const libTestUtils::TestRecordFeature*>(element->getInputFeature());
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
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::AddModifierCommand command("Test command", 51, modData.clone());

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(AddModifierCommandTest, failSafelyNoTarget) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
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
