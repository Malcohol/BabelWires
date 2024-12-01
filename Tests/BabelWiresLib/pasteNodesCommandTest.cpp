#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/pasteNodesCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveNodeCommand.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNode.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNode.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

TEST(PasteNodesCommandTest, executeAndUndoEmptyProject) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData projectData;

    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(babelwires::pathToString(sourceFilePath.m_filePath),
                             babelwires::pathToString(targetFilePath.m_filePath));
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath);

    babelwires::PasteNodesCommand command("Test command", std::move(projectData));

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();
 
    const auto checkForProjectData = [&testEnvironment]() {
        // If there are no clashes, expect the IDs in the data to be respected.
        const babelwires::Node* sourceElement =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_sourceNodeId);
        EXPECT_NE(sourceElement, nullptr);
        const babelwires::Node* processor =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_processorId);
        ASSERT_NE(processor, nullptr);
        const babelwires::Node* targetElement =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_targetNodeId);
        ASSERT_NE(targetElement, nullptr);

        // Confirm that some modifiers were pasted too.
        ASSERT_NE(processor->getEdits().findModifier(testUtils::TestProcessorInputOutputType::s_pathToInt), nullptr);
        EXPECT_FALSE(
            processor->getEdits().findModifier(testUtils::TestProcessorInputOutputType::s_pathToInt)->isFailed());
        ASSERT_NE(targetElement->getEdits().findModifier(testUtils::getTestFileElementPathToInt0()), nullptr);
        EXPECT_FALSE(targetElement->getEdits().findModifier(testUtils::getTestFileElementPathToInt0())->isFailed());
    };
    checkForProjectData();

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(testEnvironment.m_project.getNode(testUtils::TestProjectData::c_sourceNodeId), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getNode(testUtils::TestProjectData::c_processorId), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getNode(testUtils::TestProjectData::c_targetNodeId), nullptr);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    checkForProjectData();
}

TEST(PasteNodesCommandTest, executeAndUndoDuplicateData) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData originalProjectData;
    testEnvironment.m_project.setProjectData(originalProjectData);

    testUtils::TestProjectData projectData;

    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    // It's OK for sources to be duplicated. It's not so great for targets, but it won't affect this test.
    projectData.setFilePaths(babelwires::pathToString(sourceFilePath.m_filePath),
                             babelwires::pathToString(targetFilePath.m_filePath));
    originalProjectData.setFilePaths(babelwires::pathToString(sourceFilePath.m_filePath),
                                     babelwires::pathToString(targetFilePath.m_filePath));
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath);

    {
        // Confirm the original data applied as expected.
        const babelwires::Node* sourceElement =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_sourceNodeId);
        EXPECT_NE(sourceElement, nullptr);
        const babelwires::Node* processor =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_processorId);
        ASSERT_NE(processor, nullptr);
        const babelwires::Node* targetElement =
            testEnvironment.m_project.getNode(testUtils::TestProjectData::c_targetNodeId);
        ASSERT_NE(targetElement, nullptr);
        ASSERT_NE(processor->getEdits().findModifier(testUtils::TestProcessorInputOutputType::s_pathToInt), nullptr);
        EXPECT_FALSE(processor->getEdits().findModifier(testUtils::TestProcessorInputOutputType::s_pathToInt)->isFailed());
    };

    babelwires::PasteNodesCommand command("Test command", std::move(projectData));

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    const auto checkForProjectData = [&testEnvironment](bool isAfterCommandExecuted) {
        const babelwires::Node* originalSourceElement = nullptr;
        const babelwires::Node* originalProcessor = nullptr;
        const babelwires::Node* originalTargetElement = nullptr;
        const babelwires::Node* newSourceElement = nullptr;
        const babelwires::Node* newProcessor = nullptr;
        const babelwires::Node* newTargetElement = nullptr;

        for (const auto& pair : testEnvironment.m_project.getNodes()) {
            if (pair.second->as<babelwires::SourceFileNode>()) {
                if (pair.first == testUtils::TestProjectData::c_sourceNodeId) {
                    EXPECT_EQ(originalSourceElement, nullptr);
                    originalSourceElement = pair.second.get();
                } else {
                    EXPECT_EQ(newSourceElement, nullptr);
                    newSourceElement = pair.second.get();
                }
            } else if (pair.second->as<babelwires::ProcessorNode>()) {
                if (pair.first == testUtils::TestProjectData::c_processorId) {
                    EXPECT_EQ(originalProcessor, nullptr);
                    originalProcessor = pair.second.get();
                } else {
                    EXPECT_EQ(newProcessor, nullptr);
                    newProcessor = pair.second.get();
                }
            } else if (pair.second->as<babelwires::TargetFileNode>()) {
                if (pair.first == testUtils::TestProjectData::c_targetNodeId) {
                    EXPECT_EQ(originalTargetElement, nullptr);
                    originalTargetElement = pair.second.get();
                } else {
                    EXPECT_EQ(newTargetElement, nullptr);
                    newTargetElement = pair.second.get();
                }
            } else {
                // Unexpected element.
                EXPECT_TRUE(false);
            }
        }
        EXPECT_NE(originalSourceElement, nullptr);
        EXPECT_NE(originalProcessor, nullptr);
        EXPECT_NE(originalTargetElement, nullptr);
        if (isAfterCommandExecuted) {
            EXPECT_NE(newSourceElement, nullptr);
            EXPECT_NE(newProcessor, nullptr);
            EXPECT_NE(newTargetElement, nullptr);
            const babelwires::Modifier* modifier =
                newProcessor->getEdits().findModifier(testUtils::TestProcessorInputOutputType::s_pathToInt);
            ASSERT_NE(modifier, nullptr);
            EXPECT_FALSE(modifier->isFailed());
            const babelwires::ConnectionModifierData* modData =
                modifier->getModifierData().as<babelwires::ConnectionModifierData>();
            ASSERT_NE(modData, nullptr);
            EXPECT_EQ(modData->m_sourceId, newSourceElement->getNodeId());
        } else {
            EXPECT_EQ(newSourceElement, nullptr);
            EXPECT_EQ(newProcessor, nullptr);
            EXPECT_EQ(newTargetElement, nullptr);
        }
    };

    checkForProjectData(true);

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    checkForProjectData(false);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    checkForProjectData(true);
}

namespace {
    // You can paste data back into the same project, and any input connections to the new elements
    // will get wired back up.
    // If you paste into a different project, however, these connections are ignored.
    void testSourceElementsOutsideProjectData(bool isPastingIntoSameProject) {
        testUtils::TestEnvironment testEnvironment;

        testUtils::TestComplexRecordElementData sourceElementData;
        babelwires::NodeId sourceNodeId =
            testEnvironment.m_project.addNode(sourceElementData);

        const babelwires::NodeId newNodeId = sourceNodeId + 1;
        EXPECT_EQ(testEnvironment.m_project.getNode(newNodeId), nullptr);

        // Create project data with an input elementId matching the element in the project.
        babelwires::ProjectData projectData;

        if (isPastingIntoSameProject) {
            projectData.m_projectId = testEnvironment.m_project.getProjectId();
        }

        testUtils::TestComplexRecordElementData targetElementData;
        {
            babelwires::ConnectionModifierData modifierData;
            modifierData.m_targetPath = targetElementData.getPathToRecordInt0();
            modifierData.m_sourcePath = sourceElementData.getPathToRecordInt0();
            modifierData.m_sourceId = sourceNodeId;
            
            targetElementData.m_modifiers.emplace_back(modifierData.clone());
            targetElementData.m_id = newNodeId;
            projectData.m_nodes.emplace_back(targetElementData.clone());
        }

        babelwires::PasteNodesCommand command("Test command", std::move(projectData));

        EXPECT_TRUE(command.initialize(testEnvironment.m_project));

        command.execute(testEnvironment.m_project);
        testEnvironment.m_project.process();

        const auto checkForProjectData = [&testEnvironment, newNodeId, targetElementData, isPastingIntoSameProject]() {
            // The newNodeId, should be available.
            const babelwires::Node* newElement = testEnvironment.m_project.getNode(newNodeId);
            ASSERT_NE(newElement, nullptr);

            const babelwires::Modifier* modifier =
                newElement->getEdits().findModifier(targetElementData.getPathToRecordInt0());
            if (isPastingIntoSameProject) {
                ASSERT_NE(modifier, nullptr);
                EXPECT_FALSE(modifier->isFailed());
            } else {
                EXPECT_EQ(modifier, nullptr);
            }
        };

        checkForProjectData();

        command.undo(testEnvironment.m_project);
        testEnvironment.m_project.process();

        EXPECT_EQ(testEnvironment.m_project.getNode(newNodeId), nullptr);

        command.execute(testEnvironment.m_project);
        testEnvironment.m_project.process();

        checkForProjectData();
    }
} // namespace

TEST(PasteNodesCommandTest, sourceElementsOutsideProjectDataSameProject) {
    testSourceElementsOutsideProjectData(true);
}

TEST(PasteNodesCommandTest, sourceElementsOutsideProjectDataDifferentProject) {
    testSourceElementsOutsideProjectData(false);
}
