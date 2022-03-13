#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/addEntryToArrayCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

TEST(AddEntryToArrayCommandTest, executeAndUndoAtIndex) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestRootFeature* inputFeature =
        element->getInputFeature()->as<const libTestUtils::TestRootFeature>();
    ASSERT_NE(inputFeature, nullptr);

    const auto checkModifiers = [element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(libTestUtils::TestRootFeature::s_pathToArray);
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRootFeature::s_pathToArray, 1);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoAtEnd) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const libTestUtils::TestRootFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);

    // 2 is not a current entry, and therefore means "at end".
    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRootFeature::s_pathToArray, 2);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoPriorModifier) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = libTestUtils::TestRootFeature::s_pathToArray;
        arrayInitialization.m_size = 3;
        context.m_project.addModifier(elementId, arrayInitialization);
    }
    const libTestUtils::TestRootFeature* inputFeature =
        element->getInputFeature()->as<const libTestUtils::TestRootFeature>();
    ASSERT_NE(inputFeature, nullptr);

    const auto checkModifiers = [element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(libTestUtils::TestRootFeature::s_pathToArray);
        EXPECT_NE(arrayModifier, nullptr);
    };

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(false);

    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRootFeature::s_pathToArray, 1);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(false);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);
}

TEST(AddEntryToArrayCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::AddEntryToArrayCommand command("Test command", 51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), -1);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(AddEntryToArrayCommandTest, failSafelyNoArray) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::AddEntryToArrayCommand command("Test command", 51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), -1);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(AddEntryToArrayCommandTest, failSafelyOutOfRange) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const auto* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto* inputFeature = element->getInputFeature()->as<const libTestUtils::TestRootFeature>();
    ASSERT_NE(inputFeature, nullptr);
    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRootFeature::s_pathToArray, 3);

    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(AddEntryToArrayCommandTest, executeAndUndoWithValues) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const libTestUtils::TestRootFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);

    getInputFeature()->m_arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->set(3);
    getInputFeature()->m_arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->set(-18);

    // insert at the end
    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRootFeature::s_pathToArray, 2);

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->get(), 3);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->get(), -18);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->get(), 3);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->get(), -18);
}
