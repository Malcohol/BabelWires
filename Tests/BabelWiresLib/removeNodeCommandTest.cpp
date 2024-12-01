#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/removeNodeCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

TEST(RemoveNodeCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData projectData;

    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(babelwires::pathToString(sourceFilePath.m_filePath), babelwires::pathToString(targetFilePath.m_filePath));
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    testEnvironment.m_project.setProjectData(projectData);
    testEnvironment.m_project.process();

    EXPECT_NE(testEnvironment.m_project.getNode(testUtils::TestProjectData::c_processorId), nullptr);

    const auto checkElements = [&testEnvironment](bool isCommandExecuted) {
        const babelwires::Node* processor =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_processorId);
        if (isCommandExecuted) {
            ASSERT_EQ(processor, nullptr);
        } else {
            ASSERT_NE(processor, nullptr);
        }

        const babelwires::Node* targetElement =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_targetElementId);
        ASSERT_NE(targetElement, nullptr);
        const babelwires::Modifier* targetModifier =
            targetElement->getEdits().findModifier(testUtils::getTestFileElementPathToInt0());
        if (isCommandExecuted) {
            ASSERT_EQ(targetModifier, nullptr);
        } else {
            ASSERT_NE(targetModifier, nullptr);
            EXPECT_FALSE(targetModifier->isFailed());
        }
    };

    checkElements(false);

    babelwires::RemoveNodeCommand command("Test command", testUtils::TestProjectData::c_processorId);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    checkElements(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    checkElements(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    checkElements(true);
}

TEST(RemoveNodeCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueAssignmentData modData(babelwires::IntValue(86));
    modData.m_targetPath = babelwires::Path::deserializeFromString("qqq/zzz");

    babelwires::RemoveNodeCommand command("Test command", 57);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(RemoveNodeCommandTest, subsumption) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;

    const babelwires::ElementId element1Id =
        testEnvironment.m_project.addNode(elementData);
    const babelwires::ElementId element2Id =
        testEnvironment.m_project.addNode(elementData);

    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = elementData.getPathToRecordInt0();
        modData.m_sourcePath = elementData.getPathToRecordInt0();
        modData.m_sourceId = element1Id;

        testEnvironment.m_project.addModifier(element2Id, modData);
    }

    testEnvironment.m_project.process();

    babelwires::RemoveNodeCommand firstCommand("Test command", element1Id);

    auto secondCommand = std::make_unique<babelwires::RemoveNodeCommand>("Test command", element2Id);

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, false));
    firstCommand.subsume(std::move(secondCommand));

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(testEnvironment.m_project.getNode(element1Id), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getNode(element2Id), nullptr);

    // Confirm that the move was subsumed
    firstCommand.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_NE(testEnvironment.m_project.getNode(element1Id), nullptr);
    EXPECT_NE(testEnvironment.m_project.getNode(element2Id), nullptr);
    {
        const babelwires::Modifier* modifier = testEnvironment.m_project.getNode(element2Id)
                                                   ->getEdits()
                                                   .findModifier(elementData.getPathToRecordInt0());
        EXPECT_NE(modifier, nullptr);
        EXPECT_FALSE(modifier->isFailed());
    }

    firstCommand.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(testEnvironment.m_project.getNode(element1Id), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getNode(element2Id), nullptr);
}
