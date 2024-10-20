#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addEntriesToArrayCommand.hpp>

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

TEST(AddEntryToArrayCommandTest, executeAndUndoAtIndex) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestArrayElementData());
    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    const auto getArrayFeature = [element]() {
        return element->getInputFeature()->as<babelwires::ValueFeature>();
    };

    const auto checkModifiers = [element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(testUtils::TestArrayElementData::getPathToArray());
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                                 testUtils::TestArrayElementData::getPathToArray(), 1);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize + 1);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize + 1);
    checkModifiers(true);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoAtEnd) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestArrayElementData());
    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    const auto getArrayFeature = [element]() {
        return element->getInputFeature()->as<babelwires::ValueFeature>();
    };

    const auto checkModifiers = [element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(testUtils::TestArrayElementData::getPathToArray());
        if (isCommandExecuted) {
            EXPECT_NE(arrayModifier, nullptr);
        } else {
            EXPECT_EQ(arrayModifier, nullptr);
        }
    };

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    /// Insert one beyond the last element.
    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                                 testUtils::TestArrayElementData::getPathToArray(),
                                                 testUtils::TestSimpleArrayType::s_defaultSize);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize + 1);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize + 1);
    checkModifiers(true);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoPriorModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestArrayElementData());
    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    {
        babelwires::ArraySizeModifierData arrayInitialization;
        arrayInitialization.m_pathToFeature = testUtils::TestArrayElementData::getPathToArray();
        arrayInitialization.m_size = testUtils::TestSimpleArrayType::s_nonDefaultSize;
        testEnvironment.m_project.addModifier(elementId, arrayInitialization);
    }
    testEnvironment.m_project.process();

    const auto getArrayFeature = [element]() {
        return element->getInputFeature()->as<babelwires::ValueFeature>();
    };

    const auto checkModifiers = [element](bool isCommandExecuted) {
        const babelwires::Modifier* arrayModifier =
            element->findModifier(testUtils::TestArrayElementData::getPathToArray());
        EXPECT_NE(arrayModifier, nullptr);
    };

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_nonDefaultSize);
    checkModifiers(false);

    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                                 testUtils::TestArrayElementData::getPathToArray(), 1);

    EXPECT_EQ(command.getName(), "Test command");

    testEnvironment.m_project.process();
    EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_nonDefaultSize + 1);
    checkModifiers(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_nonDefaultSize);
    checkModifiers(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_nonDefaultSize + 1);
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

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestSimpleRecordElementData());

    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                                 babelwires::FeaturePath::deserializeFromString("qqq/zzz"), -1);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));
}

TEST(AddEntryToArrayCommandTest, failSafelyOutOfRange) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestArrayElementData());
    const auto* element = testEnvironment.m_project.getFeatureElement(elementId);
    ASSERT_NE(element, nullptr);

    const auto getArrayFeature = [element]() {
        return element->getInputFeature()->as<babelwires::ValueFeature>();
    };

    ASSERT_NE(getArrayFeature(), nullptr);
    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);

    babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                                 testUtils::TestArrayElementData::getPathToArray(),
                                                 testUtils::TestSimpleArrayType::s_maximumSize + 5);

    EXPECT_FALSE(command.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoWithValues) {
    for (auto p : std::vector<std::tuple<int, int>>{{testUtils::TestSimpleArrayType::s_defaultSize, 0}, {0, 1}}) {
        const unsigned int insertLocation = std::get<0>(p);
        const unsigned int offsetForValues = std::get<1>(p);

        testUtils::TestEnvironment testEnvironment;

        // Initialize with some values.
        testUtils::TestArrayElementData elementData;
        {
            // This is in the default size of the array, so should be unaffected.
            babelwires::ValueAssignmentData intAssignment(babelwires::IntValue(3));
            intAssignment.m_pathToFeature = testUtils::TestArrayElementData::getPathToArray_1();
            elementData.m_modifiers.emplace_back(intAssignment.clone());
        }
        {
            // This is in the default size of the array, so should be unaffected.
            babelwires::ValueAssignmentData intAssignment(babelwires::IntValue(-18));
            intAssignment.m_pathToFeature = testUtils::TestArrayElementData::getPathToArray_2();
            elementData.m_modifiers.emplace_back(intAssignment.clone());
        }

        const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
        auto* element = testEnvironment.m_project.getFeatureElement(elementId);
        ASSERT_NE(element, nullptr);

        const auto getArrayFeature = [element]() {
            return element->getInputFeature()->as<babelwires::ValueFeature>();
        };

        ASSERT_NE(getArrayFeature(), nullptr);

        EXPECT_GT(getArrayFeature()->getNumFeatures(), 2);

        // insert at the end
        babelwires::AddEntriesToArrayCommand command("Test command", elementId,
                                                     testUtils::TestArrayElementData::getPathToArray(),
                                                     insertLocation);

        testEnvironment.m_project.process();

        EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);
        EXPECT_EQ(getArrayFeature()->getFeature(1)->as<babelwires::ValueFeature>()->getValue(),
                  babelwires::IntValue(3));
        EXPECT_EQ(getArrayFeature()->getFeature(2)->as<babelwires::ValueFeature>()->getValue(),
                  babelwires::IntValue(-18));

        EXPECT_TRUE(command.initializeAndExecute(testEnvironment.m_project));

        testEnvironment.m_project.process();

        EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize + 1);
        EXPECT_EQ(getArrayFeature()->getFeature(offsetForValues + 1)->as<babelwires::ValueFeature>()->getValue(),
                  babelwires::IntValue(3));
        EXPECT_EQ(getArrayFeature()->getFeature(offsetForValues + 2)->as<babelwires::ValueFeature>()->getValue(),
                  babelwires::IntValue(-18));

        command.undo(testEnvironment.m_project);
        testEnvironment.m_project.process();

        EXPECT_EQ(getArrayFeature()->getNumFeatures(), testUtils::TestSimpleArrayType::s_defaultSize);

        EXPECT_EQ(getArrayFeature()->getFeature(1)->as<babelwires::ValueFeature>()->getValue(),
                  babelwires::IntValue(3));
        EXPECT_EQ(getArrayFeature()->getFeature(2)->as<babelwires::ValueFeature>()->getValue(),
                  babelwires::IntValue(-18));
    }
}
