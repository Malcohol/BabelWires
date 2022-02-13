#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/resizeElementCommand.hpp"

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

TEST(ResizeElementCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    babelwires::ResizeElementCommand command("Test command", elementId, babelwires::UiSize{113});
    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(context.m_project));
    command.execute(context.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 113);

    command.undo(context.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 77);

    command.execute(context.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 113);
}

TEST(ResizeElementCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::ResizeElementCommand command("Test command", 57, babelwires::UiSize{113});

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(ResizeElementCommandTest, subsumeMoves) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    babelwires::ResizeElementCommand firstCommand("Test command", elementId, babelwires::UiSize{113});

    EXPECT_TRUE(firstCommand.initializeAndExecute(context.m_project));

    auto secondCommand =
        std::make_unique<babelwires::ResizeElementCommand>("Test Move", elementId, babelwires::UiSize{188});

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, true));

    firstCommand.subsume(std::move(secondCommand));

    // Confirm that the move was subsumed
    firstCommand.undo(context.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 77);

    firstCommand.execute(context.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 188);
}

TEST(ResizeElementCommandTest, subsumeMovesDelay) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    babelwires::ResizeElementCommand firstCommand("Test command", elementId, babelwires::UiSize{113});

    EXPECT_TRUE(firstCommand.initializeAndExecute(context.m_project));

    auto secondCommand =
        std::make_unique<babelwires::ResizeElementCommand>("Test Move", elementId, babelwires::UiSize{188});
    secondCommand->setTimestamp(firstCommand.getTimestamp() +
                                babelwires::CommandTimestamp::duration(std::chrono::seconds(2)));

    EXPECT_FALSE(firstCommand.shouldSubsume(*secondCommand, true));
}