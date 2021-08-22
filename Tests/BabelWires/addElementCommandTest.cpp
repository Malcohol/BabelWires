#include <gtest/gtest.h>

#include "BabelWires/Commands/addElementCommand.hpp"

#include "BabelWires/Commands/moveElementCommand.hpp"
#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"

TEST(AddElementCommandTest, executeAndUndo) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::AddElementCommand command("Test command", std::make_unique<libTestUtils::TestFeatureElementData>());

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);

    const babelwires::FeatureElement* newElement = context.m_project.getFeatureElement(command.getElementId());
    ASSERT_NE(newElement, nullptr);
    EXPECT_NE(newElement->asA<libTestUtils::TestFeatureElement>(), nullptr);

    command.undo(context.m_project);

    EXPECT_EQ(context.m_project.getFeatureElement(command.getElementId()), nullptr);

    command.execute(context.m_project);

    const babelwires::FeatureElement* restoredElement = context.m_project.getFeatureElement(command.getElementId());
    ASSERT_NE(restoredElement, nullptr);
    EXPECT_NE(restoredElement->asA<libTestUtils::TestFeatureElement>(), nullptr);
}

TEST(AddElementCommandTest, subsumeMoves) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::AddElementCommand addCommand("Test command", std::make_unique<libTestUtils::TestFeatureElementData>());

    EXPECT_TRUE(addCommand.initializeAndExecute(context.m_project));

    auto moveCommand = std::make_unique<babelwires::MoveElementCommand>("Test Move", addCommand.getElementId(),
                                                                        babelwires::UiPosition{14, 88});

    EXPECT_TRUE(addCommand.shouldSubsume(*moveCommand, true));

    addCommand.subsume(std::move(moveCommand));

    // Confirm that the move was subsumed
    addCommand.undo(context.m_project);
    addCommand.execute(context.m_project);
    const auto* element =
        context.m_project.getFeatureElement(addCommand.getElementId())->asA<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, 14);
    EXPECT_EQ(element->getUiPosition().m_y, 88);
}
