#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/pasteElementsCommand.hpp"

#include "BabelWiresLib/Commands/moveElementCommand.hpp"
#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElement.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testEnvironment.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectData.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "Tests/TestUtils/tempFilePath.hpp"

TEST(PasteElementsCommandTest, executeAndUndoEmptyProject) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    libTestUtils::TestProjectData projectData;

    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    libTestUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath);

    babelwires::PasteElementsCommand command("Test command", std::move(projectData));

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    const auto checkForProjectData = [&testEnvironment]() {
        // If there are no clashes, expect the IDs in the data to be respected.
        const babelwires::FeatureElement* sourceElement =
            testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_sourceElementId);
        EXPECT_NE(sourceElement, nullptr);
        const babelwires::FeatureElement* processor =
            testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_processorId);
        ASSERT_NE(processor, nullptr);
        const babelwires::FeatureElement* targetElement =
            testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_targetElementId);
        ASSERT_NE(targetElement, nullptr);

        // Confirm that some modifiers were pasted too.
        ASSERT_NE(processor->getEdits().findModifier(libTestUtils::TestRootFeature::s_pathToInt), nullptr);
        EXPECT_FALSE(processor->getEdits().findModifier(libTestUtils::TestRootFeature::s_pathToInt)->isFailed());
        ASSERT_NE(targetElement->getEdits().findModifier(libTestUtils::TestFileFeature::s_pathToIntChild), nullptr);
        EXPECT_FALSE(
            targetElement->getEdits().findModifier(libTestUtils::TestFileFeature::s_pathToIntChild)->isFailed());
    };
    checkForProjectData();

    command.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_sourceElementId), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_processorId), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_targetElementId), nullptr);

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    checkForProjectData();
}

TEST(PasteElementsCommandTest, executeAndUndoDuplicateData) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestEnvironment testEnvironment;

    libTestUtils::TestProjectData originalProjectData;
    testEnvironment.m_project.setProjectData(originalProjectData);

    libTestUtils::TestProjectData projectData;

    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    // It's OK for sources to be duplicated. It's not so great for targets, but it won't affect this test.
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    originalProjectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    libTestUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath);

    {
        // Confirm the original data applied as expected.
        const babelwires::FeatureElement* sourceElement =
            testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_sourceElementId);
        EXPECT_NE(sourceElement, nullptr);
        const babelwires::FeatureElement* processor =
            testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_processorId);
        ASSERT_NE(processor, nullptr);
        const babelwires::FeatureElement* targetElement =
            testEnvironment.m_project.getFeatureElement(libTestUtils::TestProjectData::c_targetElementId);
        ASSERT_NE(targetElement, nullptr);
        ASSERT_NE(processor->getEdits().findModifier(libTestUtils::TestRootFeature::s_pathToInt), nullptr);
        EXPECT_FALSE(processor->getEdits().findModifier(libTestUtils::TestRootFeature::s_pathToInt)->isFailed());
    };

    babelwires::PasteElementsCommand command("Test command", std::move(projectData));

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(testEnvironment.m_project));

    command.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    const auto checkForProjectData = [&testEnvironment](bool isAfterCommandExecuted) {
        const babelwires::FeatureElement* originalSourceElement = nullptr;
        const babelwires::FeatureElement* originalProcessor = nullptr;
        const babelwires::FeatureElement* originalTargetElement = nullptr;
        const babelwires::FeatureElement* newSourceElement = nullptr;
        const babelwires::FeatureElement* newProcessor = nullptr;
        const babelwires::FeatureElement* newTargetElement = nullptr;

        for (const auto& pair : testEnvironment.m_project.getElements()) {
            if (pair.second->as<babelwires::SourceFileElement>()) {
                if (pair.first == libTestUtils::TestProjectData::c_sourceElementId) {
                    EXPECT_EQ(originalSourceElement, nullptr);
                    originalSourceElement = pair.second.get();
                } else {
                    EXPECT_EQ(newSourceElement, nullptr);
                    newSourceElement = pair.second.get();
                }
            } else if (pair.second->as<babelwires::ProcessorElement>()) {
                if (pair.first == libTestUtils::TestProjectData::c_processorId) {
                    EXPECT_EQ(originalProcessor, nullptr);
                    originalProcessor = pair.second.get();
                } else {
                    EXPECT_EQ(newProcessor, nullptr);
                    newProcessor = pair.second.get();
                }
            } else if (pair.second->as<babelwires::TargetFileElement>()) {
                if (pair.first == libTestUtils::TestProjectData::c_targetElementId) {
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
                newProcessor->getEdits().findModifier(libTestUtils::TestRootFeature::s_pathToInt);
            ASSERT_NE(modifier, nullptr);
            EXPECT_FALSE(modifier->isFailed());
            const babelwires::ConnectionModifierData* modData =
                modifier->getModifierData().as<babelwires::ConnectionModifierData>();
            ASSERT_NE(modData, nullptr);
            EXPECT_EQ(modData->m_sourceId, newSourceElement->getElementId());
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
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestEnvironment testEnvironment;

        babelwires::ElementId sourceElementId =
            testEnvironment.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

        const babelwires::ElementId newElementId = sourceElementId + 1;
        EXPECT_EQ(testEnvironment.m_project.getFeatureElement(newElementId), nullptr);

        // Create project data with an input elementId matching the element in the project.
        babelwires::ProjectData projectData;

        if (isPastingIntoSameProject) {
            projectData.m_projectId = testEnvironment.m_project.getProjectId();
        }

        {
            babelwires::ConnectionModifierData modifierData;
            modifierData.m_pathToFeature = libTestUtils::TestRootFeature::s_pathToInt2;
            modifierData.m_pathToSourceFeature = libTestUtils::TestRootFeature::s_pathToInt2;
            modifierData.m_sourceId = sourceElementId;
            libTestUtils::TestFeatureElementData elementData;
            elementData.m_modifiers.emplace_back(modifierData.clone());
            elementData.m_id = newElementId;
            projectData.m_elements.emplace_back(elementData.clone());
        }

        babelwires::PasteElementsCommand command("Test command", std::move(projectData));

        EXPECT_TRUE(command.initialize(testEnvironment.m_project));

        command.execute(testEnvironment.m_project);
        testEnvironment.m_project.process();

        const auto checkForProjectData = [&testEnvironment, newElementId, isPastingIntoSameProject]() {
            // The newElementId, should be available.
            const babelwires::FeatureElement* newElement = testEnvironment.m_project.getFeatureElement(newElementId);
            ASSERT_NE(newElement, nullptr);

            const babelwires::Modifier* modifier =
                newElement->getEdits().findModifier(libTestUtils::TestRootFeature::s_pathToInt2);
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

        EXPECT_EQ(testEnvironment.m_project.getFeatureElement(newElementId), nullptr);

        command.execute(testEnvironment.m_project);
        testEnvironment.m_project.process();

        checkForProjectData();
    }
} // namespace

TEST(PasteElementsCommandTest, sourceElementsOutsideProjectDataSameProject) {
    testSourceElementsOutsideProjectData(true);
}

TEST(PasteElementsCommandTest, sourceElementsOutsideProjectDataDifferentProject) {
    testSourceElementsOutsideProjectData(false);
}
