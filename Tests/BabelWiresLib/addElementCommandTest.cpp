#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/addElementCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveElementCommand.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElement.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(AddElementCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::AddElementCommand command("Test command", std::make_unique<testUtils::TestSimpleRecordElementData>());

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);

    const babelwires::FeatureElement* newElement = testEnvironment.m_project.getFeatureElement(command.getElementId());
    ASSERT_NE(newElement, nullptr);
    EXPECT_NE(newElement->as<babelwires::ValueElement>(), nullptr);

    command.undo(testEnvironment.m_project);

    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(command.getElementId()), nullptr);

    command.execute(testEnvironment.m_project);

    const babelwires::FeatureElement* restoredElement = testEnvironment.m_project.getFeatureElement(command.getElementId());
    ASSERT_NE(restoredElement, nullptr);
    EXPECT_NE(restoredElement->as<babelwires::ValueElement>(), nullptr);
}

TEST(AddElementCommandTest, subsumeMoves) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::AddElementCommand addCommand("Test command", std::make_unique<testUtils::TestSimpleRecordElementData>());

    EXPECT_TRUE(addCommand.initializeAndExecute(testEnvironment.m_project));

    auto moveCommand = std::make_unique<babelwires::MoveElementCommand>("Test Move", addCommand.getElementId(),
                                                                        babelwires::UiPosition{14, 88});

    EXPECT_TRUE(addCommand.shouldSubsume(*moveCommand, true));

    addCommand.subsume(std::move(moveCommand));

    // Confirm that the move was subsumed
    addCommand.undo(testEnvironment.m_project);
    addCommand.execute(testEnvironment.m_project);
    const auto* element =
        testEnvironment.m_project.getFeatureElement(addCommand.getElementId())->as<babelwires::ValueElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiPosition().m_x, 14);
    EXPECT_EQ(element->getUiPosition().m_y, 88);
}
