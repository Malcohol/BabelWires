#include <gtest/gtest.h>

#include "BabelWires/Commands/setExpandedCommand.hpp"

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"

TEST(SetExpandedCommandTest, executeAndUndoTrue) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, libTestUtils::TestRecordFeature::s_pathToArray,
                                           true);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));

    EXPECT_TRUE(command.initialize(context.m_project));
    command.execute(context.m_project);

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));

    command.undo(context.m_project);

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));

    command.execute(context.m_project);

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));
}

TEST(SetExpandedCommandTest, executeAndUndoFalse) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_expandedPaths.emplace_back(libTestUtils::TestRecordFeature::s_pathToArray);

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);

    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    babelwires::SetExpandedCommand command("Test command", elementId, libTestUtils::TestRecordFeature::s_pathToArray,
                                           false);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));

    EXPECT_TRUE(command.initialize(context.m_project));
    command.execute(context.m_project);

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));

    command.undo(context.m_project);

    EXPECT_TRUE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));

    command.execute(context.m_project);

    EXPECT_FALSE(element->isExpanded(libTestUtils::TestRecordFeature::s_pathToArray));
}

TEST(SetExpandedCommandTest, failSafelyNoElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::SetExpandedCommand command("Test command", 51,
                                           babelwires::FeaturePath::deserializeFromString("qqq/zzz"), true);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoFeature) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::SetExpandedCommand command("Test command", 51,
                                           babelwires::FeaturePath::deserializeFromString("qqq/zzz"), true);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(SetExpandedCommandTest, failSafelyNoCompound) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;
    babelwires::SetExpandedCommand command("Test command", 51, libTestUtils::TestRecordFeature::s_pathToInt2, true);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 51;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId, 51);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}
