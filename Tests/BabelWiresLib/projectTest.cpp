#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/FeatureElements/SourceFileElement/sourceFileElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/TargetFileElement/targetFileElementData.hpp>
#include <BabelWiresLib/Project/FeatureElements/ProcessorElement/processorElementData.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFeatureElement.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

#include <fstream>

TEST(ProjectTest, setAndExtractProjectData) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData projectData;

    // Make real data for project.
    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath);

    testEnvironment.m_project.setProjectData(projectData);
    // Processing ensures the featurePaths get resolved.
    testEnvironment.m_project.process();

    babelwires::ProjectData extractedData = testEnvironment.m_project.extractProjectData();

    testUtils::TestProjectData::testProjectData(testEnvironment.m_projectContext, extractedData);
}

TEST(ProjectTest, projectId) {
    testUtils::TestEnvironment testEnvironment;

    // The test testEnvironment has a built-in newly constructed project.
    const babelwires::ProjectId projectId = testEnvironment.m_project.getProjectId();

    EXPECT_NE(projectId, babelwires::INVALID_PROJECT_ID);

    babelwires::Project project2(testEnvironment.m_projectContext, testEnvironment.m_log);

    const babelwires::ProjectId project2Id = project2.getProjectId();

    EXPECT_NE(projectId, project2Id);

    testEnvironment.m_project.clear();

    EXPECT_NE(testEnvironment.m_project.getProjectId(), babelwires::INVALID_PROJECT_ID);
    EXPECT_NE(testEnvironment.m_project.getProjectId(), projectId);
    EXPECT_NE(testEnvironment.m_project.getProjectId(), project2Id);
}

TEST(ProjectTest, addGetAndRemoveElement) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const babelwires::FeatureElement* element = testEnvironment.m_project.getFeatureElement(elementId);
    EXPECT_NE(element, nullptr);
    EXPECT_NE(element->as<testUtils::TestFeatureElement>(), nullptr);
    EXPECT_TRUE(element->isChanged(babelwires::FeatureElement::Changes::FeatureElementIsNew));

    testEnvironment.m_project.removeElement(elementId);

    const babelwires::FeatureElement* element2 = testEnvironment.m_project.getFeatureElement(elementId);
    EXPECT_EQ(element2, nullptr);

    const auto& removedElements = testEnvironment.m_project.getRemovedElements();

    EXPECT_EQ(removedElements.size(), 1);
    const auto it = removedElements.find(elementId);
    ASSERT_NE(it, removedElements.end());
    EXPECT_EQ(it->first, elementId);
    EXPECT_EQ(it->second.get(), element);

    testEnvironment.m_project.clearChanges();

    EXPECT_EQ(testEnvironment.m_project.getRemovedElements().size(), 0);
}

TEST(ProjectTest, addAndRemoveLocalModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const babelwires::FeaturePath pathToFeature = testUtils::TestRootFeature::s_pathToArray_1;
    EXPECT_EQ(element->findModifier(pathToFeature), nullptr);

    babelwires::ValueAssignmentData modData(babelwires::IntValue(199));
    modData.m_pathToFeature = pathToFeature;

    testEnvironment.m_project.addModifier(elementId, modData);

    const babelwires::Modifier* const modifier = element->findModifier(pathToFeature);
    EXPECT_NE(modifier, nullptr);
    EXPECT_TRUE(modifier->isChanged(babelwires::Modifier::Changes::ModifierIsNew));

    testEnvironment.m_project.removeModifier(elementId, pathToFeature);
    EXPECT_EQ(element->findModifier(pathToFeature), nullptr);
}

TEST(ProjectTest, addAndRemoveConnectionModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId sourceElementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* sourceElement =
        testEnvironment.m_project.getFeatureElement(sourceElementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::ElementId targetElementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* targetElement =
        testEnvironment.m_project.getFeatureElement(targetElementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(targetElement, nullptr);

    const babelwires::FeaturePath pathToTargetFeature = testUtils::TestRootFeature::s_pathToArray_1;
    const babelwires::FeaturePath pathToSourceFeature = testUtils::TestRootFeature::s_pathToArray_0;

    EXPECT_EQ(sourceElement->findModifier(pathToTargetFeature), nullptr);
    babelwires::ConnectionModifierData modData;
    modData.m_pathToFeature = pathToTargetFeature;
    modData.m_sourceId = sourceElementId;
    modData.m_pathToSourceFeature = pathToSourceFeature;

    testEnvironment.m_project.addModifier(targetElementId, modData);

    const babelwires::Modifier* const modifier = targetElement->findModifier(pathToTargetFeature);
    EXPECT_NE(modifier, nullptr);
    EXPECT_TRUE(modifier->isChanged(babelwires::Modifier::Changes::ModifierIsNew));

    testEnvironment.m_project.removeModifier(targetElementId, pathToTargetFeature);
    EXPECT_EQ(sourceElement->findModifier(pathToTargetFeature), nullptr);
}

TEST(ProjectTest, addAndRemoveArrayEntriesSimple) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    const babelwires::FeaturePath pathToArray = testUtils::TestRootFeature::s_pathToArray;
    EXPECT_EQ(element->findModifier(pathToArray), nullptr);

    testEnvironment.m_project.process();
    // You can add at the end.
    testEnvironment.m_project.addArrayEntries(elementId, pathToArray, 2, 2, true);
    {
        const babelwires::Modifier* const modifier = element->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        ASSERT_TRUE(modifier->getModifierData().as<babelwires::ArraySizeModifierData>());
        EXPECT_EQ(static_cast<const babelwires::ArraySizeModifierData&>(modifier->getModifierData()).m_size, 4);
    }

    testEnvironment.m_project.process();
    testEnvironment.m_project.addArrayEntries(elementId, pathToArray, 1, 2, true);
    {
        const babelwires::Modifier* const modifier = element->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        EXPECT_EQ(modifier->getModifierData().as<babelwires::ArraySizeModifierData>()->m_size, 6);
    }

    testEnvironment.m_project.process();
    testEnvironment.m_project.removeArrayEntries(elementId, pathToArray, 2, 2, true);
    {
        const babelwires::Modifier* const modifier = element->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        EXPECT_EQ(modifier->getModifierData().as<babelwires::ArraySizeModifierData>()->m_size, 4);
    }

    testEnvironment.m_project.process();
    // The false means the modifier will be removed.
    testEnvironment.m_project.removeArrayEntries(elementId, pathToArray, 2, 2, false);
    EXPECT_EQ(element->findModifier(pathToArray), nullptr);
}

TEST(ProjectTest, addAndRemoveArrayEntriesModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    babelwires::ValueAssignmentData modData(babelwires::IntValue(702));
    modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToArray_1;
    testEnvironment.m_project.addModifier(elementId, modData);

    const babelwires::FeaturePath pathToArray = testUtils::TestRootFeature::s_pathToArray;
    EXPECT_EQ(element->findModifier(pathToArray), nullptr);

    testEnvironment.m_project.process();

    // Add after.
    testEnvironment.m_project.addArrayEntries(elementId, pathToArray, 2, 2, true);
    EXPECT_NE(element->findModifier(testUtils::TestRootFeature::s_pathToArray_1), nullptr);

    testEnvironment.m_project.process();

    // Add before.
    testEnvironment.m_project.addArrayEntries(elementId, pathToArray, 1, 3, true);
    EXPECT_EQ(element->findModifier(testUtils::TestRootFeature::s_pathToArray_1), nullptr);
    EXPECT_NE(element->findModifier(testUtils::TestRootFeature::s_pathToArray_4), nullptr);

    testEnvironment.m_project.process();

    // Remove after
    testEnvironment.m_project.removeArrayEntries(elementId, pathToArray, 5, 1, true);
    EXPECT_NE(element->findModifier(testUtils::TestRootFeature::s_pathToArray_4), nullptr);

    // Remove before.
    testEnvironment.m_project.process();
    testEnvironment.m_project.removeArrayEntries(elementId, pathToArray, 0, 2, true);
    EXPECT_NE(element->findModifier(testUtils::TestRootFeature::s_pathToArray_2), nullptr);

    // We don't test removal of modifiers, because responsibility for doing this is left to the
    // commands. (In fact, add/removeArrayEntries already does more than it should.)
}

TEST(ProjectTest, addAndRemoveArrayEntriesSource) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId sourceElementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* sourceElement =
        testEnvironment.m_project.getFeatureElement(sourceElementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::ElementId targetElementId =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const testUtils::TestFeatureElement* targetElement =
        testEnvironment.m_project.getFeatureElement(targetElementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(targetElement, nullptr);

    const babelwires::FeaturePath pathToTargetFeature = testUtils::TestRootFeature::s_pathToInt;

    const babelwires::FeaturePath pathToArray = testUtils::TestRootFeature::s_pathToArray;
    EXPECT_EQ(sourceElement->findModifier(pathToArray), nullptr);

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = pathToTargetFeature;
        modData.m_sourceId = sourceElementId;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToArray_1;
        testEnvironment.m_project.addModifier(targetElementId, modData);
    }
    const babelwires::ConnectionModifierData& connectionData = 
        *targetElement->findModifier(pathToTargetFeature)->getModifierData().as<babelwires::ConnectionModifierData>();

    testEnvironment.m_project.process();

    // The new elements are after the modifier, so it should be in the same place.
    testEnvironment.m_project.addArrayEntries(sourceElementId, pathToArray, 2, 2, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, testUtils::TestRootFeature::s_pathToArray_1);

    testEnvironment.m_project.process();

    // The new elements should mean the modifier moved.
    testEnvironment.m_project.addArrayEntries(sourceElementId, pathToArray, 1, 3, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, testUtils::TestRootFeature::s_pathToArray_4);

    testEnvironment.m_project.process();

    // Remove after
    testEnvironment.m_project.removeArrayEntries(sourceElementId, pathToArray, 5, 1, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, testUtils::TestRootFeature::s_pathToArray_4);

    // Remove before.
    testEnvironment.m_project.process();
    testEnvironment.m_project.removeArrayEntries(sourceElementId, pathToArray, 0, 2, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, testUtils::TestRootFeature::s_pathToArray_2);

    // We don't test removal of modifiers, because responsibility for doing this is left to the
    // commands. (In fact, add/removeArrayEntries already does more than it should.)
}

TEST(ProjectTest, uiProperties) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestFeatureElementData testFeatureData;
    testFeatureData.m_uiData.m_uiPosition.m_x = 8;
    testFeatureData.m_uiData.m_uiPosition.m_y = 2;
    testFeatureData.m_uiData.m_uiSize.m_width = 77;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testFeatureData);

    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    EXPECT_EQ(element->getUiPosition().m_x, 8);
    EXPECT_EQ(element->getUiPosition().m_y, 2);
    EXPECT_EQ(element->getUiSize().m_width, 77);
    testEnvironment.m_project.setElementPosition(elementId, babelwires::UiPosition{10, 12});

    EXPECT_EQ(element->getUiPosition().m_x, 10);
    EXPECT_EQ(element->getUiPosition().m_y, 12);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    testEnvironment.m_project.setElementContentsSize(elementId, babelwires::UiSize{66});

    EXPECT_EQ(element->getUiPosition().m_x, 10);
    EXPECT_EQ(element->getUiPosition().m_y, 12);
    EXPECT_EQ(element->getUiSize().m_width, 66);
}

TEST(ProjectTest, elementIds) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    EXPECT_NE(elementId, babelwires::INVALID_ELEMENT_ID);
    ASSERT_NE(elementId, 3);

    const testUtils::TestFeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element, nullptr);

    testUtils::TestFeatureElementData elementData;
    elementData.m_id = 3;

    const babelwires::ElementId elementId1 = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId1, 3);

    const testUtils::TestFeatureElement* element1 =
        testEnvironment.m_project.getFeatureElement(elementId1)->as<testUtils::TestFeatureElement>();
    ASSERT_NE(element1, nullptr);
    EXPECT_EQ(element1->getElementData().m_id, elementId1);
    EXPECT_NE(element, element1);

    const babelwires::ElementId elementId2 = testEnvironment.m_project.addFeatureElement(elementData);
    EXPECT_NE(elementId2, babelwires::INVALID_ELEMENT_ID);
    EXPECT_NE(elementId2, elementId1);

    const testUtils::TestFeatureElement* element2 =
        testEnvironment.m_project.getFeatureElement(elementId2)->as<testUtils::TestFeatureElement>();
    EXPECT_NE(element2, nullptr);
    EXPECT_NE(element1, element2);
}

TEST(ProjectTest, reloadSource) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TempFilePath tempFilePath("testSource." + testUtils::TestSourceFileFormat::getFileExtension());

    testUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 14);

    babelwires::SourceFileElementData sourceFileData;
    sourceFileData.m_filePath = tempFilePath;
    sourceFileData.m_factoryIdentifier = testUtils::TestSourceFileFormat::getThisIdentifier();

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(sourceFileData);
    const babelwires::FeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::FeatureElement>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element->getOutputFeature(), nullptr);
    ASSERT_NE(element->getOutputFeature()->as<const testUtils::TestFileFeature>(), nullptr);
    EXPECT_EQ(static_cast<const testUtils::TestFileFeature*>(element->getOutputFeature())->m_intChildFeature->get(),
              14);

    testUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 88);

    testEnvironment.m_project.tryToReloadSource(elementId);
    EXPECT_EQ(static_cast<const testUtils::TestFileFeature*>(element->getOutputFeature())->m_intChildFeature->get(),
              88);

    testUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 55);

    testEnvironment.m_project.tryToReloadAllSources();
    EXPECT_EQ(static_cast<const testUtils::TestFileFeature*>(element->getOutputFeature())->m_intChildFeature->get(),
              55);

    tempFilePath.tryRemoveFile();

    testEnvironment.m_log.clear();
    testEnvironment.m_project.tryToReloadSource(elementId);
    EXPECT_TRUE(element->isFailed());
    testEnvironment.m_log.hasSubstringIgnoreCase("could not be loaded");
}

TEST(ProjectTest, saveTarget) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TempFilePath tempFilePath("testTarget." + testUtils::TestSourceFileFormat::getFileExtension());

    babelwires::TargetFileElementData targetFileData;
    targetFileData.m_filePath = tempFilePath;
    targetFileData.m_factoryIdentifier = testUtils::TestTargetFileFormat::getThisIdentifier();

    const babelwires::ElementId elementId = testEnvironment.m_project.addFeatureElement(targetFileData);
    babelwires::FeatureElement* element =
        testEnvironment.m_project.getFeatureElement(elementId)->as<babelwires::FeatureElement>();
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element->getInputFeature(), nullptr);
    auto* inputFeature = element->getInputFeature()->as<testUtils::TestFileFeature>();
    ASSERT_NE(inputFeature, nullptr);

    inputFeature->m_intChildFeature->set(47);

    testEnvironment.m_project.tryToSaveTarget(elementId);

    EXPECT_EQ(testUtils::TestSourceFileFormat::getFileData(tempFilePath), 47);

    inputFeature->m_intChildFeature->set(30);
    testEnvironment.m_project.tryToSaveTarget(elementId);
    EXPECT_EQ(testUtils::TestSourceFileFormat::getFileData(tempFilePath), 30);

    inputFeature->m_intChildFeature->set(79);
    testEnvironment.m_project.tryToSaveAllTargets();
    EXPECT_EQ(testUtils::TestSourceFileFormat::getFileData(tempFilePath), 79);

    std::ofstream lockThisStream = tempFilePath.openForWriting();

    testEnvironment.m_log.clear();
    testEnvironment.m_project.tryToSaveTarget(elementId);
    testEnvironment.m_log.hasSubstringIgnoreCase("Cannot write output");
}

TEST(ProjectTest, process) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData projectData;

    // Make real data for project.
    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    testEnvironment.m_project.setProjectData(projectData);
    testEnvironment.m_project.process();

    const babelwires::FeatureElement* sourceElement =
        testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_sourceElementId);
    ASSERT_NE(sourceElement, nullptr);
    const testUtils::TestFileFeature* sourceOutput =
        sourceElement->getOutputFeature()->as<const testUtils::TestFileFeature>();
    ASSERT_NE(sourceOutput, nullptr);

    const babelwires::FeatureElement* processor =
        testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_processorId);
    ASSERT_NE(processor, nullptr);
    const testUtils::TestRootFeature* processorInput =
        processor->getInputFeature()->as<const testUtils::TestRootFeature>();
    ASSERT_NE(processorInput, nullptr);
    const testUtils::TestRootFeature* processorOutput =
        processor->getOutputFeature()->as<const testUtils::TestRootFeature>();
    ASSERT_NE(processorOutput, nullptr);

    const babelwires::FeatureElement* targetElement =
        testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_targetElementId);
    ASSERT_NE(targetElement, nullptr);
    const testUtils::TestFileFeature* targetInput =
        targetElement->getInputFeature()->as<const testUtils::TestFileFeature>();
    ASSERT_NE(targetInput, nullptr);

    // 4rd array entry, where they count up from the input value (3).
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 6);

    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 4);
    testEnvironment.m_project.tryToReloadSource(testUtils::TestProjectData::c_sourceElementId);
    testEnvironment.m_project.process();
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 7);

    // Removing this modifier will mean the output array is shorter than the modifier at the target requires.
    testEnvironment.m_project.removeModifier(testUtils::TestProjectData::c_processorId,
                                     testUtils::TestRootFeature::s_pathToInt);
    testEnvironment.m_project.process();
    EXPECT_TRUE(targetElement->findModifier(testUtils::TestFileFeature::s_pathToIntChild)->isFailed());
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 0);

    babelwires::ElementId newProcId;
    {
        babelwires::ProcessorElementData procData;
        procData.m_factoryIdentifier = testUtils::TestProcessorFactory::getThisIdentifier();

        babelwires::ValueAssignmentData modData(babelwires::IntValue(5));
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        procData.m_modifiers.emplace_back(modData.clone());

        newProcId = testEnvironment.m_project.addFeatureElement(procData);
    }

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt;
        modData.m_sourceId = newProcId;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(testUtils::TestProjectData::c_processorId, modData);
    }
    testEnvironment.m_project.process();
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 8);
}

TEST(ProjectTest, dependencyLoop) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId1 =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId elementId2 =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId elementId3 =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const babelwires::FeatureElement* element1 = testEnvironment.m_project.getFeatureElement(elementId1);
    const babelwires::FeatureElement* element2 = testEnvironment.m_project.getFeatureElement(elementId2);
    const babelwires::FeatureElement* element3 = testEnvironment.m_project.getFeatureElement(elementId3);

    testEnvironment.m_project.process();

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_sourceId = elementId2;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(elementId1, modData);
    }
    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_sourceId = elementId3;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(elementId2, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_FALSE(element1->isFailed());
    EXPECT_FALSE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element1->isInDependencyLoop());
    EXPECT_FALSE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_sourceId = elementId1;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(elementId3, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_TRUE(element1->isFailed());
    EXPECT_TRUE(element2->isFailed());
    EXPECT_TRUE(element3->isFailed());
    EXPECT_TRUE(element1->isInDependencyLoop());
    EXPECT_TRUE(element2->isInDependencyLoop());
    EXPECT_TRUE(element3->isInDependencyLoop());

    testEnvironment.m_project.removeModifier(elementId2, testUtils::TestRootFeature::s_pathToInt2);
    testEnvironment.m_project.process();

    EXPECT_FALSE(element1->isFailed());
    EXPECT_FALSE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element1->isInDependencyLoop());
    EXPECT_FALSE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());
}

// Check that one dependency loop does not prevent other elements from processing correctly.
TEST(ProjectTest, dependencyLoopAndProcessing) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::ElementId elementId1 =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId elementId2 =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId elementId3 =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());
    const babelwires::ElementId elementId4 =
        testEnvironment.m_project.addFeatureElement(testUtils::TestFeatureElementData());

    const babelwires::FeatureElement* element1 = testEnvironment.m_project.getFeatureElement(elementId1);
    const babelwires::FeatureElement* element2 = testEnvironment.m_project.getFeatureElement(elementId2);
    const babelwires::FeatureElement* element3 = testEnvironment.m_project.getFeatureElement(elementId3);
    const babelwires::FeatureElement* element4 = testEnvironment.m_project.getFeatureElement(elementId4);

    testEnvironment.m_project.process();

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_sourceId = elementId2;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(elementId1, modData);
    }
    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_sourceId = elementId4;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(elementId3, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_FALSE(element1->isFailed());
    EXPECT_FALSE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element4->isFailed());
    EXPECT_FALSE(element1->isInDependencyLoop());
    EXPECT_FALSE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());
    EXPECT_FALSE(element4->isInDependencyLoop());

    {
        babelwires::ConnectionModifierData modData;
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        modData.m_sourceId = elementId1;
        modData.m_pathToSourceFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(elementId2, modData);
    }
    {
        babelwires::ValueAssignmentData modData(babelwires::IntValue(16));
        modData.m_pathToFeature = testUtils::TestRootFeature::s_pathToInt2;
        testEnvironment.m_project.addModifier(elementId4, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_TRUE(element1->isFailed());
    EXPECT_TRUE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element4->isFailed());
    EXPECT_TRUE(element1->isInDependencyLoop());
    EXPECT_TRUE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());
    EXPECT_FALSE(element4->isInDependencyLoop());

    ASSERT_NE(element3->getOutputFeature()->as<testUtils::TestRootFeature>(), nullptr);
    EXPECT_EQ(element3->getOutputFeature()->as<testUtils::TestRootFeature>()->m_intFeature2->get(), 16);              

    testEnvironment.m_project.removeModifier(elementId2, testUtils::TestRootFeature::s_pathToInt2);
    testEnvironment.m_project.process();

    EXPECT_FALSE(element1->isFailed());
    EXPECT_FALSE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element4->isFailed());
    EXPECT_FALSE(element1->isInDependencyLoop());
    EXPECT_FALSE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());
    EXPECT_FALSE(element4->isInDependencyLoop());
}

TEST(ProjectTest, updateWithAvailableIds) {
    testUtils::TestEnvironment testEnvironment;

    std::vector<babelwires::ElementId> idsToCheck0 = {testUtils::TestProjectData::c_sourceElementId,
                                                      testUtils::TestProjectData::c_processorId,
                                                      testUtils::TestProjectData::c_targetElementId};
    testEnvironment.m_project.updateWithAvailableIds(idsToCheck0);
    EXPECT_EQ(idsToCheck0[0], testUtils::TestProjectData::c_sourceElementId);
    EXPECT_EQ(idsToCheck0[1], testUtils::TestProjectData::c_processorId);
    EXPECT_EQ(idsToCheck0[2], testUtils::TestProjectData::c_targetElementId);

    testUtils::TestProjectData projectData;
    testEnvironment.m_project.setProjectData(projectData);

    std::vector<babelwires::ElementId> idsToCheck1 = {testUtils::TestProjectData::c_sourceElementId,
                                                      testUtils::TestProjectData::c_processorId,
                                                      testUtils::TestProjectData::c_targetElementId};
    testEnvironment.m_project.updateWithAvailableIds(idsToCheck1);
    EXPECT_NE(idsToCheck1[0], testUtils::TestProjectData::c_sourceElementId);
    EXPECT_NE(idsToCheck1[1], testUtils::TestProjectData::c_processorId);
    EXPECT_NE(idsToCheck1[2], testUtils::TestProjectData::c_targetElementId);
    EXPECT_NE(idsToCheck1[0], idsToCheck1[1]);
    EXPECT_NE(idsToCheck1[1], idsToCheck1[2]);
    EXPECT_NE(idsToCheck1[2], idsToCheck1[0]);

    for (int i = 0; i < 3; ++i) {
        testUtils::TestFeatureElementData data;
        data.m_id = idsToCheck1[i];
        EXPECT_EQ(testEnvironment.m_project.addFeatureElement(data), idsToCheck1[i]);
    }

    std::vector<babelwires::ElementId> idsToCheck2 = {testUtils::TestProjectData::c_sourceElementId,
                                                      testUtils::TestProjectData::c_processorId,
                                                      testUtils::TestProjectData::c_targetElementId};
    testEnvironment.m_project.updateWithAvailableIds(idsToCheck2);
    EXPECT_NE(idsToCheck2[0], testUtils::TestProjectData::c_sourceElementId);
    EXPECT_NE(idsToCheck2[1], testUtils::TestProjectData::c_processorId);
    EXPECT_NE(idsToCheck2[2], testUtils::TestProjectData::c_targetElementId);
    EXPECT_NE(idsToCheck2[0], idsToCheck1[0]);
    EXPECT_NE(idsToCheck2[1], idsToCheck1[1]);
    EXPECT_NE(idsToCheck2[2], idsToCheck1[2]);
    EXPECT_NE(idsToCheck2[0], idsToCheck2[1]);
    EXPECT_NE(idsToCheck2[1], idsToCheck2[2]);
    EXPECT_NE(idsToCheck2[2], idsToCheck2[0]);
}

TEST(ProjectTest, processWithFailure) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData projectData;
    // Ensure this element fails completely.
    projectData.m_elements[1]->m_factoryIdentifier = "flerg";

    // Make real data for project.
    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    testEnvironment.m_project.setProjectData(projectData);
    testEnvironment.m_project.process();

    const babelwires::FeatureElement* sourceElement =
        testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_sourceElementId);
    ASSERT_NE(sourceElement, nullptr);
    const testUtils::TestFileFeature* sourceOutput =
        sourceElement->getOutputFeature()->as<const testUtils::TestFileFeature>();
    ASSERT_NE(sourceOutput, nullptr);

    const babelwires::FeatureElement* processor =
        testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_processorId);
    ASSERT_NE(processor, nullptr);
    ASSERT_TRUE(processor->isFailed());

    const babelwires::FeatureElement* targetElement =
        testEnvironment.m_project.getFeatureElement(testUtils::TestProjectData::c_targetElementId);
    ASSERT_NE(targetElement, nullptr);
    const testUtils::TestFileFeature* targetInput =
        targetElement->getInputFeature()->as<const testUtils::TestFileFeature>();
    ASSERT_NE(targetInput, nullptr);
}
