#include <gtest/gtest.h>

#include "BabelWires/Commands/addEntryToArrayCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"

// TODO test where there's a pre-existing ArraySizeModifier

TEST(AddEntryToArrayCommandTest, executeAndUndoAtIndex) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->asA<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const libTestUtils::TestRecordFeature* inputFeature =
        element->getInputFeature()->asA<const libTestUtils::TestRecordFeature>();
    ASSERT_NE(inputFeature, nullptr);

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRecordFeature::s_pathToArray, 1);

    EXPECT_EQ(command.getName(), "Test command");

    context.m_project.process();
    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);

    command.undo(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    command.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 3);
}

TEST(AddEntryToArrayCommandTest, executeAndUndoAtEnd) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->asA<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    const auto getInputFeature = [element]() {
        return element->getInputFeature()->asA<const libTestUtils::TestRecordFeature>();
    };
    ASSERT_NE(getInputFeature(), nullptr);

    EXPECT_EQ(getInputFeature()->m_arrayFeature->getNumFeatures(), 2);

    // -1 means "at end".
    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRecordFeature::s_pathToArray, -1);

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

TEST(AddEntryToArrayCommandTest, failSafelyNoElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::AddEntryToArrayCommand command("Test command", 51,
                                               babelwires::FeaturePath::deserializeFromString("qqq/zzz"), -1);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(AddEntryToArrayCommandTest, failSafelyNoArray) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
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
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const auto* element =
        context.m_project.getFeatureElement(elementId)->asA<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const auto* inputFeature = element->getInputFeature()->asA<const libTestUtils::TestRecordFeature>();
    ASSERT_NE(inputFeature, nullptr);
    EXPECT_EQ(inputFeature->m_arrayFeature->getNumFeatures(), 2);

    babelwires::AddEntryToArrayCommand command("Test command", elementId,
                                               libTestUtils::TestRecordFeature::s_pathToArray, 3);

    EXPECT_FALSE(command.initialize(context.m_project));
}
