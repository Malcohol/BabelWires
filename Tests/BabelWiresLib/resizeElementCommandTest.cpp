#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/resizeElementCommand.hpp>

#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

TEST(ResizeElementCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    babelwires::ResizeElementCommand command("Test command", elementId, babelwires::UiSize{113});
    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));
    command.execute(testEnvironment.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 113);

    command.undo(testEnvironment.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 77);

    command.execute(testEnvironment.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 113);
}

TEST(ResizeElementCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::ResizeElementCommand command("Test command", 57, babelwires::UiSize{113});

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(ResizeElementCommandTest, subsumeMoves) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    babelwires::ResizeElementCommand firstCommand("Test command", elementId, babelwires::UiSize{113});

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    auto secondCommand =
        std::make_unique<babelwires::ResizeElementCommand>("Test Move", elementId, babelwires::UiSize{188});

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, true));

    firstCommand.subsume(std::move(secondCommand));

    // Confirm that the move was subsumed
    firstCommand.undo(testEnvironment.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 77);

    firstCommand.execute(testEnvironment.m_project);

    EXPECT_EQ(element->getUiSize().m_width, 188);
}

TEST(ResizeElementCommandTest, subsumeMovesDelay) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData elementData;
    elementData.m_uiData.m_uiSize = babelwires::UiSize{77};

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(elementData);
    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    babelwires::ResizeElementCommand firstCommand("Test command", elementId, babelwires::UiSize{113});

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    auto secondCommand =
        std::make_unique<babelwires::ResizeElementCommand>("Test Move", elementId, babelwires::UiSize{188});
    secondCommand->setTimestamp(firstCommand.getTimestamp() +
                                babelwires::CommandTimestamp::duration(std::chrono::seconds(2)));

    EXPECT_FALSE(firstCommand.shouldSubsume(*secondCommand, true));
}
