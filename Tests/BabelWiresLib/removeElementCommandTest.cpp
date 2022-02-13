#include <gtest/gtest.h>

#include "BabelWiresLib/Commands/removeElementCommand.hpp"

#include "BabelWiresLib/Commands/moveElementCommand.hpp"
#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElement.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElement.hpp"
#include "BabelWiresLib/Project/Modifiers/modifier.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectData.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

#include "Tests/TestUtils/tempFilePath.hpp"

TEST(RemoveElementCommandTest, executeAndUndo) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestProjectData projectData;

    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    libTestUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    context.m_project.setProjectData(projectData);
    context.m_project.process();

    EXPECT_NE(context.m_project.getFeatureElement(libTestUtils::TestProjectData::c_processorId), nullptr);

    const auto checkElements = [&context](bool isCommandExecuted) {
        const babelwires::FeatureElement* processor =
            context.m_project.getFeatureElement(libTestUtils::TestProjectData::c_processorId);
        if (isCommandExecuted) {
            ASSERT_EQ(processor, nullptr);
        } else {
            ASSERT_NE(processor, nullptr);
        }

        const babelwires::FeatureElement* targetElement =
            context.m_project.getFeatureElement(libTestUtils::TestProjectData::c_targetElementId);
        ASSERT_NE(targetElement, nullptr);
        const babelwires::Modifier* targetModifier =
            targetElement->getEdits().findModifier(libTestUtils::TestFileFeature::s_pathToIntChild);
        if (isCommandExecuted) {
            ASSERT_EQ(targetModifier, nullptr);
        } else {
            ASSERT_NE(targetModifier, nullptr);
            EXPECT_FALSE(targetModifier->isFailed());
        }
    };

    checkElements(false);

    babelwires::RemoveElementCommand command("Test command", libTestUtils::TestProjectData::c_processorId);

    EXPECT_EQ(command.getName(), "Test command");

    EXPECT_TRUE(command.initialize(context.m_project));

    command.execute(context.m_project);
    context.m_project.process();

    checkElements(true);

    command.undo(context.m_project);
    context.m_project.process();

    checkElements(false);

    command.execute(context.m_project);
    context.m_project.process();

    checkElements(true);
}

TEST(RemoveElementCommandTest, failSafelyNoElement) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");
    modData.m_value = 86;

    babelwires::RemoveElementCommand command("Test command", 57);

    context.m_project.process();
    EXPECT_FALSE(command.initialize(context.m_project));
}

TEST(RemoveElementCommandTest, subsumption) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId element1Id =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId element2Id =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_sourceId = element1Id;

        context.m_project.addModifier(element2Id, modData);
    }

    context.m_project.process();

    babelwires::RemoveElementCommand firstCommand("Test command", element1Id);

    auto secondCommand = std::make_unique<babelwires::RemoveElementCommand>("Test command", element2Id);

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, false));
    firstCommand.subsume(std::move(secondCommand));

    EXPECT_TRUE(firstCommand.initializeAndExecute(context.m_project));

    EXPECT_EQ(context.m_project.getFeatureElement(element1Id), nullptr);
    EXPECT_EQ(context.m_project.getFeatureElement(element2Id), nullptr);

    // Confirm that the move was subsumed
    firstCommand.undo(context.m_project);
    context.m_project.process();

    EXPECT_NE(context.m_project.getFeatureElement(element1Id), nullptr);
    EXPECT_NE(context.m_project.getFeatureElement(element2Id), nullptr);
    {
        const babelwires::Modifier* modifier = context.m_project.getFeatureElement(element2Id)
                                                   ->getEdits()
                                                   .findModifier(libTestUtils::TestRecordFeature::s_pathToInt2);
        EXPECT_NE(modifier, nullptr);
        EXPECT_FALSE(modifier->isFailed());
    }

    firstCommand.execute(context.m_project);
    context.m_project.process();

    EXPECT_EQ(context.m_project.getFeatureElement(element1Id), nullptr);
    EXPECT_EQ(context.m_project.getFeatureElement(element2Id), nullptr);
}