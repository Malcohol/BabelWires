#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/addElementCommand.hpp"

#include "BabelWiresLib/Commands/moveElementCommand.hpp"
#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"

TEST(AddElementCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    babelwires::AddElementCommand command("Test command", std::make_unique<libTestUtils::TestFeatureElementData>());

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);

    const babelwires::FeatureElement* newElement = testEnvironment.m_project.getFeatureElement(command.getElementId());
    ASSERT_NE(newElement, nullptr);
    EXPECT_NE(newElement->as<libTestUtils::TestFeatureElement>(), nullptr);

    command.undo(testEnvironment.m_project);

    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(command.getElementId()), nullptr);

    command.execute(testEnvironment.m_project);

    const babelwires::FeatureElement* restoredElement = testEnvironment.m_project.getFeatureElement(command.getElementId());
    ASSERT_NE(restoredElement, nullptr);
    EXPECT_NE(restoredElement->as<libTestUtils::TestFeatureElement>(), nullptr);
}

TEST(AddElementCommandTest, subsumeMoves) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    babelwires::AddElementCommand addCommand("Test command", std::make_unique<libTestUtils::TestFeatureElementData>());

    EXPECT_TRUE(addCommand.initializeAndExecute(testEnvironment.m_project));

    auto moveCommand = std::make_unique<babelwires::MoveElementCommand>("Test Move", addCommand.getElementId(),
                                                                        babelwires::UiPosition{14, 88});

    EXPECT_TRUE(addCommand.shouldSubsume(*moveCommand, true));

    addCommand.subsume(std::move(moveCommand));

    // Confirm that the move was subsumed
    addCommand.undo(testEnvironment.m_project);
    addCommand.execute(testEnvironment.m_project);
    const auto* element =
        testEnvironment.m_project.getFeatureElement(addCommand.getElementId())->as<libTestUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, 14);
    EXPECT_EQ(element->getUiPosition().m_y, 88);
}
