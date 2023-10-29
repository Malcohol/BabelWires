#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

TEST(AddEntryToArrayCommandTest, executeAndUndoAtIndex) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const testUtils::TestRootFeature* inputFeature =
        element->getInputFeature()->as<const testUtils::TestRootFeature>();
    ASSERT_NE(inputFeature, nullptr);

    const auto checkModifiers = [element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray);
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                               testUtils::TestRootFeature::s_pathToArray, 1);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(true);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoAtEnd) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);

    // 2 is not a current entry, and therefore means "at end".
    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                               testUtils::TestRootFeature::s_pathToArray, 2);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoPriorModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray;
        arrayInitialization.m_size = 3;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    const testUtils::TestRootFeature* inputFeature =
        element->getInputFeature()->as<const testUtils::TestRootFeature>();
    ASSERT_NE(inputFeature, nullptr);

    const auto checkModifiers = [element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(testUtils::TestRootFeature::s_pathToArray);
        EXPECT_NE(arrayModifier, nullptr);
    };

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(false);

    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                               testUtils::TestRootFeature::s_pathToArray, 1);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 4);
    checkModifiers(true);
}

TEST(AddEntryToArrayCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::AddEntriesToArrayCommand command("Test command", 51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), -1);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddEntryToArrayCommandTest, failSafelyNoArray) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::AddEntriesToArrayCommand command("Test command", 51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), -1);

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddEntryToArrayCommandTest, failSafelyOutOfRange) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const auto* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto* inputFeature = element->getInputFeature()->as<const testUtils::TestRootFeature>();
    ASSERT_NE(inputFeature, nullptr);
    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                               testUtils::TestRootFeature::s_pathToArray, 3);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddEntryToArrayCommandTest, executeAndUndoWithValues) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->as<const testUtils::TestRootFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);

    getInputFeature()->m_arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->set(3);
    getInputFeature()->m_arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->set(-18);

    // insert at the end
    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                               testUtils::TestRootFeature::s_pathToArray, 2);

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 3);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->get(), 3);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->get(), -18);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(0)->as<babelwires::IntFeature>()->get(), 3);
    EXPECT_EQ(getInputFeature()->m_arrayFeature->getFeature(1)->as<babelwires::IntFeature>()->get(), -18);
}
