#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/moveElementCommand.hpp"

#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

TEST(MoveElementCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiPosition = babelwires::UiPosition{-14, -15};

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, -14);
    EXPECT_EQ(element->getUiPosition().m_y, -15);

    babelwires::MoveElementCommand command("Test command", elementId, babelwires::UiPosition{100, 12});
    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(context.m_project));
    command.execute(context.m_project);

    EXPECT_EQ(element->getUiPosition().m_x, 100);
    EXPECT_EQ(element->getUiPosition().m_y, 12);

    command.undo(context.m_project);

    EXPECT_EQ(element->getUiPosition().m_x, -14);
    EXPECT_EQ(element->getUiPosition().m_y, -15);

    command.execute(context.m_project);

    EXPECT_EQ(element->getUiPosition().m_x, 100);
    EXPECT_EQ(element->getUiPosition().m_y, 12);
}

TEST(MoveElementCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::MoveElementCommand command("Test command", 57, babelwires::UiPosition{100, 12});

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(MoveElementCommandTest, subsumeMoves) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiPosition = babelwires::UiPosition{-14, -15};

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, -14);
    EXPECT_EQ(element->getUiPosition().m_y, -15);

    babelwires::MoveElementCommand firstCommand("Test command", elementId, babelwires::UiPosition{100, 12});

    EXPECT_TRUE(firstCommand.initializeAndExecute(context.m_project));

    auto secondCommand =
        std::make_unique<babelwires::MoveElementCommand>("Test Move", elementId, babelwires::UiPosition{14, 88});

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, true));

    firstCommand.subsume(std::move(secondCommand));

    // Confirm that the move was subsumed
    firstCommand.undo(context.m_project);

    EXPECT_EQ(element->getUiPosition().m_x, -14);
    EXPECT_EQ(element->getUiPosition().m_y, -15);

    firstCommand.execute(context.m_project);

    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, 14);
    EXPECT_EQ(element->getUiPosition().m_y, 88);
}

TEST(MoveElementCommandTest, subsumeMovesDelay) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiPosition = babelwires::UiPosition{-14, -15};

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(elementData);
    const libTestUtils::TestFeatureElement* element =
        context.m_project.getFeatureElement(elementId)->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, -14);
    EXPECT_EQ(element->getUiPosition().m_y, -15);

    babelwires::MoveElementCommand firstCommand("Test command", elementId, babelwires::UiPosition{100, 12});

    EXPECT_TRUE(firstCommand.initializeAndExecute(context.m_project));

    auto secondCommand =
        std::make_unique<babelwires::MoveElementCommand>("Test Move", elementId, babelwires::UiPosition{14, 88});
    secondCommand->setTimestamp(firstCommand.getTimestamp() +
                                babelwires::CommandTimestamp::duration(std::chrono::seconds(2)));

    EXPECT_FALSE(firstCommand.shouldSubsume(*secondCommand, true));
}
