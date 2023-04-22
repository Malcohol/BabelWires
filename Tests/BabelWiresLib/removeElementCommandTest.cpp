#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Commands/removeElementCommand.hpp>

#include <BabelWiresLib/Project/Commands/moveElementCommand.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/processorElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/sourceFileElement.hpp>
#include <BabelWiresLib/Project/FeatureElements/targetFileElement.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

TEST(RemoveElementCommandTest, executeAndUndo) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData projectData;

    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    testEnvironment.m_project.setProjectData(projectData);
    testEnvironment.m_project.process();

    EXPECT_NE(testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_processorId), nullptr);

    const auto checkElements = [&testEnvironment](bool isCommandExecuted) {
        const babelwires::FeatureElement* processor =
            testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_processorId);
        if (isCommandExecuted) {
            ASSERT_EQ(processor, nullptr);
        } else {
            ASSERT_NE(processor, nullptr);
        }

        const babelwires::FeatureElement* targetElement =
            testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_targetElementId);
        ASSERT_NE(targetElement, nullptr);
        const babelwires::Modifier* targetModifier =
            targetElement->getEdits().findModifier(testUtils::TestFileFeature::s_pathToIntChild);
        if (isCommandExecuted) {
            ASSERT_EQ(targetModifier, nullptr);
        } else {
            ASSERT_NE(targetModifier, nullptr);
            EXPECT_FALSE(targetModifier->isFailed());
        }
    };

    checkElements(false);

    babelwires::RemoveElementCommand command("Test command", testUtils::TestProjectData::c_processorId);

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

TEST(RemoveElementCommandTest, failSafelyNoElement) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ValueAssignmentData modData(babelwires::IntValue(86));
    modData.m_pathToFeature = babelwires::FeaturePath::deserializeFromString("qqq/zzz");

    babelwires::RemoveElementCommand command("Test command", 57);

    testEnvironment.m_project.process();
    EXPECT_FALSE(command.initialize(testEnvironment.m_project));
}

TEST(RemoveElementCommandTest, subsumption) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId element1Id =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId element2Id =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_sourceId = element1Id;

        testEnvironment.m_project.addModifier(element2Id, modData);
    }

    testEnvironment.m_project.process();

    babelwires::RemoveElementCommand firstCommand("Test command", element1Id);

    auto secondCommand = std::make_unique<babelwires::RemoveElementCommand>("Test command", element2Id);

    EXPECT_TRUE(firstCommand.shouldSubsume(*secondCommand, false));
    firstCommand.subsume(std::move(secondCommand));

    EXPECT_TRUE(firstCommand.initializeAndExecute(testEnvironment.m_project));

    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(element1Id), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(element2Id), nullptr);

    // Confirm that the move was subsumed
    firstCommand.undo(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_NE(testEnvironment.m_project.getFeatureElement(element1Id), nullptr);
    EXPECT_NE(testEnvironment.m_project.getFeatureElement(element2Id), nullptr);
    {
        const babelwires::Modifier* modifier = testEnvironment.m_project.getFeatureElement(element2Id)
                                                   ->getEdits()
                                                   .findModifier(testUtils::TestRootFeature::s_pathToInt2);
        EXPECT_NE(modifier, nullptr);
        EXPECT_FALSE(modifier->isFailed());
    }

    firstCommand.execute(testEnvironment.m_project);
    testEnvironment.m_project.process();

    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(element1Id), nullptr);
    EXPECT_EQ(testEnvironment.m_project.getFeatureElement(element2Id), nullptr);
}
