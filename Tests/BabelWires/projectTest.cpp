#include <gtest/gtest.h>

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Project/Modifiers/modifier.hpp"
#include "BabelWires/Project/project.hpp"

#include "Tests/BabelWires/TestUtils/testFeatureElement.hpp"
#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWires/TestUtils/testProcessor.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testProjectData.hpp"

#include "Tests/TestUtils/tempFilePath.hpp"

#include <fstream>

TEST(ProjectTest, setAndExtractProjectData) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestProjectData projectData;

    // Make real data for project.
    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    libTestUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath);

    context.m_project.setProjectData(projectData);
    // Processing ensures the featurePaths get resolved.
    context.m_project.process();

    babelwires::ProjectData extractedData = context.m_project.extractProjectData();

    libTestUtils::TestProjectData::testProjectData(extractedData);
}

TEST(ProjectTest, projectId) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    // The test context has a built-in newly constructed project.
    const babelwires::ProjectId projectId = context.m_project.getProjectId();

    EXPECT_NE(projectId, babelwires::INVALID_PROJECT_ID);

    babelwires::Project project2(context.m_projectContext, context.m_log);

    const babelwires::ProjectId project2Id = project2.getProjectId();

    EXPECT_NE(projectId, project2Id);

    context.m_project.clear();

    EXPECT_NE(context.m_project.getProjectId(), babelwires::INVALID_PROJECT_ID);
    EXPECT_NE(context.m_project.getProjectId(), projectId);
    EXPECT_NE(context.m_project.getProjectId(), project2Id);
}

TEST(ProjectTest, addGetAndRemoveElement) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const babelwires::FeatureElement* element = context.m_project.getFeatureElement(elementId);
    EXPECT_NE(element, nullptr);
    EXPECT_NE(dynamic_cast<const libTestUtils::TestFeatureElement*>(element), nullptr);
    EXPECT_TRUE(element->isChanged(babelwires::FeatureElement::Changes::FeatureElementIsNew));

    context.m_project.removeElement(elementId);

    const babelwires::FeatureElement* element2 = context.m_project.getFeatureElement(elementId);
    EXPECT_EQ(element2, nullptr);

    const auto& removedElements = context.m_project.getRemovedElements();

    EXPECT_EQ(removedElements.size(), 1);
    const auto it = removedElements.find(elementId);
    ASSERT_NE(it, removedElements.end());
    EXPECT_EQ(it->first, elementId);
    EXPECT_EQ(it->second.get(), element);

    context.m_project.clearChanges();

    EXPECT_EQ(context.m_project.getRemovedElements().size(), 0);
}

TEST(ProjectTest, addAndRemoveLocalModifier) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    const babelwires::FeaturePath pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
    EXPECT_EQ(element->findModifier(pathToFeature), nullptr);

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = pathToFeature;
    modData.m_value = 199;

    context.m_project.addModifier(elementId, modData);

    const babelwires::Modifier* const modifier = element->findModifier(pathToFeature);
    EXPECT_NE(modifier, nullptr);
    EXPECT_TRUE(modifier->isChanged(babelwires::Modifier::Changes::ModifierIsNew));

    context.m_project.removeModifier(elementId, pathToFeature);
    EXPECT_EQ(element->findModifier(pathToFeature), nullptr);
}

TEST(ProjectTest, addAndRemoveConnectionModifier) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId sourceElementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* sourceElement =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(sourceElementId));
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::ElementId targetElementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* targetElement =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(targetElementId));
    ASSERT_NE(targetElement, nullptr);

    const babelwires::FeaturePath pathToTargetFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
    const babelwires::FeaturePath pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToArray_0;

    EXPECT_EQ(sourceElement->findModifier(pathToTargetFeature), nullptr);
    babelwires::AssignFromFeatureData modData;
    modData.m_pathToFeature = pathToTargetFeature;
    modData.m_sourceId = sourceElementId;
    modData.m_pathToSourceFeature = pathToSourceFeature;

    context.m_project.addModifier(targetElementId, modData);

    const babelwires::Modifier* const modifier = targetElement->findModifier(pathToTargetFeature);
    EXPECT_NE(modifier, nullptr);
    EXPECT_TRUE(modifier->isChanged(babelwires::Modifier::Changes::ModifierIsNew));

    context.m_project.removeModifier(targetElementId, pathToTargetFeature);
    EXPECT_EQ(sourceElement->findModifier(pathToTargetFeature), nullptr);
}

TEST(ProjectTest, addAndRemoveArrayEntriesSimple) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    const babelwires::FeaturePath pathToArray = libTestUtils::TestRecordFeature::s_pathToArray;
    EXPECT_EQ(element->findModifier(pathToArray), nullptr);

    context.m_project.process();
    // You can add at the end.
    context.m_project.addArrayEntries(elementId, pathToArray, 2, 2, true);
    {
        const babelwires::Modifier* const modifier = element->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        ASSERT_TRUE(dynamic_cast<const babelwires::ArrayInitializationData*>(&modifier->getModifierData()));
        EXPECT_EQ(static_cast<const babelwires::ArrayInitializationData&>(modifier->getModifierData()).m_size, 4);
    }

    context.m_project.process();
    context.m_project.addArrayEntries(elementId, pathToArray, 1, 2, true);
    {
        const babelwires::Modifier* const modifier = element->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        EXPECT_EQ(dynamic_cast<const babelwires::ArrayInitializationData&>(modifier->getModifierData()).m_size, 6);
    }

    context.m_project.process();
    context.m_project.removeArrayEntries(elementId, pathToArray, 2, 2, true);
    {
        const babelwires::Modifier* const modifier = element->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        EXPECT_EQ(dynamic_cast<const babelwires::ArrayInitializationData&>(modifier->getModifierData()).m_size, 4);
    }

    context.m_project.process();
    // The false means the modifier will be removed.
    context.m_project.removeArrayEntries(elementId, pathToArray, 2, 2, false);
    EXPECT_EQ(element->findModifier(pathToArray), nullptr);
}

TEST(ProjectTest, addAndRemoveArrayEntriesModifier) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    babelwires::IntValueAssignmentData modData;
    modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
    modData.m_value = 702;
    context.m_project.addModifier(elementId, modData);

    const babelwires::FeaturePath pathToArray = libTestUtils::TestRecordFeature::s_pathToArray;
    EXPECT_EQ(element->findModifier(pathToArray), nullptr);

    context.m_project.process();

    // Add after.
    context.m_project.addArrayEntries(elementId, pathToArray, 2, 2, true);
    EXPECT_NE(element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_1), nullptr);

    context.m_project.process();

    // Add before.
    context.m_project.addArrayEntries(elementId, pathToArray, 1, 3, true);
    EXPECT_EQ(element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_1), nullptr);
    EXPECT_NE(element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_4), nullptr);

    context.m_project.process();

    // Remove after
    context.m_project.removeArrayEntries(elementId, pathToArray, 5, 1, true);
    EXPECT_NE(element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_4), nullptr);

    // Remove before.
    context.m_project.process();
    context.m_project.removeArrayEntries(elementId, pathToArray, 0, 2, true);
    EXPECT_NE(element->findModifier(libTestUtils::TestRecordFeature::s_pathToArray_2), nullptr);

    // We don't test removal of modifiers, because responsibility for doing this is left to the
    // commands. (In fact, add/removeArrayEntries already does more than it should.)
}

TEST(ProjectTest, addAndRemoveArrayEntriesSource) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId sourceElementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* sourceElement =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(sourceElementId));
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::ElementId targetElementId =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const libTestUtils::TestFeatureElement* targetElement =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(targetElementId));
    ASSERT_NE(targetElement, nullptr);

    const babelwires::FeaturePath pathToTargetFeature = libTestUtils::TestRecordFeature::s_pathToInt;

    const babelwires::FeaturePath pathToArray = libTestUtils::TestRecordFeature::s_pathToArray;
    EXPECT_EQ(sourceElement->findModifier(pathToArray), nullptr);

    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = pathToTargetFeature;
        modData.m_sourceId = sourceElementId;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToArray_1;
        context.m_project.addModifier(targetElementId, modData);
    }
    const babelwires::AssignFromFeatureData& connectionData = dynamic_cast<const babelwires::AssignFromFeatureData&>(
        targetElement->findModifier(pathToTargetFeature)->getModifierData());

    context.m_project.process();

    // The new elements are after the modifier, so it should be in the same place.
    context.m_project.addArrayEntries(sourceElementId, pathToArray, 2, 2, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, libTestUtils::TestRecordFeature::s_pathToArray_1);

    context.m_project.process();

    // The new elements should mean the modifier moved.
    context.m_project.addArrayEntries(sourceElementId, pathToArray, 1, 3, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, libTestUtils::TestRecordFeature::s_pathToArray_4);

    context.m_project.process();

    // Remove after
    context.m_project.removeArrayEntries(sourceElementId, pathToArray, 5, 1, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, libTestUtils::TestRecordFeature::s_pathToArray_4);

    // Remove before.
    context.m_project.process();
    context.m_project.removeArrayEntries(sourceElementId, pathToArray, 0, 2, true);
    EXPECT_EQ(connectionData.m_pathToSourceFeature, libTestUtils::TestRecordFeature::s_pathToArray_2);

    // We don't test removal of modifiers, because responsibility for doing this is left to the
    // commands. (In fact, add/removeArrayEntries already does more than it should.)
}

TEST(ProjectTest, uiProperties) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestFeatureElementData testFeatureData;
    testFeatureData.m_uiData.m_uiPosition.m_x = 8;
    testFeatureData.m_uiData.m_uiPosition.m_y = 2;
    testFeatureData.m_uiData.m_uiSize.m_width = 77;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(testFeatureData);

    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    EXPECT_EQ(element->getUiPosition().m_x, 8);
    EXPECT_EQ(element->getUiPosition().m_y, 2);
    EXPECT_EQ(element->getUiSize().m_width, 77);
    context.m_project.setElementPosition(elementId, babelwires::UiPosition{10, 12});

    EXPECT_EQ(element->getUiPosition().m_x, 10);
    EXPECT_EQ(element->getUiPosition().m_y, 12);
    EXPECT_EQ(element->getUiSize().m_width, 77);

    context.m_project.setElementContentsSize(elementId, babelwires::UiSize{66});

    EXPECT_EQ(element->getUiPosition().m_x, 10);
    EXPECT_EQ(element->getUiPosition().m_y, 12);
    EXPECT_EQ(element->getUiSize().m_width, 66);
}

TEST(ProjectTest, elementIds) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    EXPECT_NE(elementId, babelwires::INVALID_ELEMENT_ID);
    ASSERT_NE(elementId, 3);

    const libTestUtils::TestFeatureElement* element =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);

    libTestUtils::TestFeatureElementData elementData;
    elementData.m_id = 3;

    const babelwires::ElementId elementId1 = context.m_project.addFeatureElement(elementData);
    EXPECT_EQ(elementId1, 3);

    const libTestUtils::TestFeatureElement* element1 =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId1));
    ASSERT_NE(element1, nullptr);
    EXPECT_EQ(element1->getElementData().m_id, elementId1);
    EXPECT_NE(element, element1);

    const babelwires::ElementId elementId2 = context.m_project.addFeatureElement(elementData);
    EXPECT_NE(elementId2, babelwires::INVALID_ELEMENT_ID);
    EXPECT_NE(elementId2, elementId1);

    const libTestUtils::TestFeatureElement* element2 =
        dynamic_cast<const libTestUtils::TestFeatureElement*>(context.m_project.getFeatureElement(elementId2));
    EXPECT_NE(element2, nullptr);
    EXPECT_NE(element1, element2);
}

TEST(ProjectTest, reloadSource) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    testUtils::TempFilePath tempFilePath("testSource." + libTestUtils::TestSourceFileFormat::getFileExtension());

    libTestUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 14);

    babelwires::SourceFileData sourceFileData;
    sourceFileData.m_filePath = tempFilePath;
    sourceFileData.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(sourceFileData);
    const babelwires::FeatureElement* element =
        dynamic_cast<const babelwires::FeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element->getOutputFeature(), nullptr);
    ASSERT_NE(dynamic_cast<const libTestUtils::TestFileFeature*>(element->getOutputFeature()), nullptr);
    EXPECT_EQ(static_cast<const libTestUtils::TestFileFeature*>(element->getOutputFeature())->m_intChildFeature->get(),
              14);

    libTestUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 88);

    context.m_project.tryToReloadSource(elementId);
    EXPECT_EQ(static_cast<const libTestUtils::TestFileFeature*>(element->getOutputFeature())->m_intChildFeature->get(),
              88);

    libTestUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 55);

    context.m_project.tryToReloadAllSources();
    EXPECT_EQ(static_cast<const libTestUtils::TestFileFeature*>(element->getOutputFeature())->m_intChildFeature->get(),
              55);

    tempFilePath.tryRemoveFile();

    context.m_log.clear();
    context.m_project.tryToReloadSource(elementId);
    EXPECT_TRUE(element->isFailed());
    context.m_log.hasSubstringIgnoreCase("could not be loaded");
}

TEST(ProjectTest, saveTarget) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    testUtils::TempFilePath tempFilePath("testTarget." + libTestUtils::TestSourceFileFormat::getFileExtension());

    babelwires::TargetFileData targetFileData;
    targetFileData.m_filePath = tempFilePath;
    targetFileData.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();

    const babelwires::ElementId elementId = context.m_project.addFeatureElement(targetFileData);
    babelwires::FeatureElement* element =
        dynamic_cast<babelwires::FeatureElement*>(context.m_project.getFeatureElement(elementId));
    ASSERT_NE(element, nullptr);
    ASSERT_NE(element->getInputFeature(), nullptr);
    auto* inputFeature = dynamic_cast<libTestUtils::TestFileFeature*>(element->getInputFeature());
    ASSERT_NE(inputFeature, nullptr);

    inputFeature->m_intChildFeature->set(47);

    context.m_project.tryToSaveTarget(elementId);

    EXPECT_EQ(libTestUtils::TestSourceFileFormat::getFileData(tempFilePath), 47);

    inputFeature->m_intChildFeature->set(30);
    context.m_project.tryToSaveTarget(elementId);
    EXPECT_EQ(libTestUtils::TestSourceFileFormat::getFileData(tempFilePath), 30);

    inputFeature->m_intChildFeature->set(79);
    context.m_project.tryToSaveAllTargets();
    EXPECT_EQ(libTestUtils::TestSourceFileFormat::getFileData(tempFilePath), 79);

    std::ofstream lockThisStream(tempFilePath);

    context.m_log.clear();
    context.m_project.tryToSaveTarget(elementId);
    context.m_log.hasSubstringIgnoreCase("Cannot write output");
}

TEST(ProjectTest, process) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestProjectData projectData;

    // Make real data for project.
    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(sourceFilePath.m_filePath.u8string(), targetFilePath.m_filePath.u8string());
    libTestUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    context.m_project.setProjectData(projectData);
    context.m_project.process();

    const babelwires::FeatureElement* sourceElement =
        context.m_project.getFeatureElement(libTestUtils::TestProjectData::c_sourceElementId);
    ASSERT_NE(sourceElement, nullptr);
    const libTestUtils::TestFileFeature* sourceOutput =
        dynamic_cast<const libTestUtils::TestFileFeature*>(sourceElement->getOutputFeature());
    ASSERT_NE(sourceOutput, nullptr);

    const babelwires::FeatureElement* processor =
        context.m_project.getFeatureElement(libTestUtils::TestProjectData::c_processorId);
    ASSERT_NE(processor, nullptr);
    const libTestUtils::TestRecordFeature* processorInput =
        dynamic_cast<const libTestUtils::TestRecordFeature*>(processor->getInputFeature());
    ASSERT_NE(processorInput, nullptr);
    const libTestUtils::TestRecordFeature* processorOutput =
        dynamic_cast<const libTestUtils::TestRecordFeature*>(processor->getOutputFeature());
    ASSERT_NE(processorOutput, nullptr);

    const babelwires::FeatureElement* targetElement =
        context.m_project.getFeatureElement(libTestUtils::TestProjectData::c_targetElementId);
    ASSERT_NE(targetElement, nullptr);
    const libTestUtils::TestFileFeature* targetInput =
        dynamic_cast<const libTestUtils::TestFileFeature*>(targetElement->getInputFeature());
    ASSERT_NE(targetInput, nullptr);

    // 4rd array entry, where they count up from the input value (3).
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 6);

    libTestUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 4);
    context.m_project.tryToReloadSource(libTestUtils::TestProjectData::c_sourceElementId);
    context.m_project.process();
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 7);

    // Removing this modifier will mean the output array is shorter than the modifier at the target requires.
    context.m_project.removeModifier(libTestUtils::TestProjectData::c_processorId,
                                     libTestUtils::TestRecordFeature::s_pathToInt);
    context.m_project.process();
    EXPECT_TRUE(targetElement->findModifier(libTestUtils::TestFileFeature::s_pathToIntChild)->isFailed());
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 0);

    babelwires::ElementId newProcId;
    {
        babelwires::ProcessorData procData;
        procData.m_factoryIdentifier = libTestUtils::TestProcessorFactory::getThisIdentifier();

        babelwires::IntValueAssignmentData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_value = 5;
        procData.m_modifiers.emplace_back(modData.clone());

        newProcId = context.m_project.addFeatureElement(procData);
    }

    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt;
        modData.m_sourceId = newProcId;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        context.m_project.addModifier(libTestUtils::TestProjectData::c_processorId, modData);
    }
    context.m_project.process();
    EXPECT_EQ(targetInput->m_intChildFeature->get(), 8);
}

TEST(ProjectTest, dependencyLoop) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId1 =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId elementId2 =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId elementId3 =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const babelwires::FeatureElement* element1 = context.m_project.getFeatureElement(elementId1);
    const babelwires::FeatureElement* element2 = context.m_project.getFeatureElement(elementId2);
    const babelwires::FeatureElement* element3 = context.m_project.getFeatureElement(elementId3);

    context.m_project.process();

    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_sourceId = elementId2;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        context.m_project.addModifier(elementId1, modData);
    }
    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_sourceId = elementId3;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        context.m_project.addModifier(elementId2, modData);
    }

    context.m_project.process();

    EXPECT_FALSE(element1->isFailed());
    EXPECT_FALSE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element1->isInDependencyLoop());
    EXPECT_FALSE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());

    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_sourceId = elementId1;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        context.m_project.addModifier(elementId3, modData);
    }

    context.m_project.process();

    EXPECT_TRUE(element1->isFailed());
    EXPECT_TRUE(element2->isFailed());
    EXPECT_TRUE(element3->isFailed());
    EXPECT_TRUE(element1->isInDependencyLoop());
    EXPECT_TRUE(element2->isInDependencyLoop());
    EXPECT_TRUE(element3->isInDependencyLoop());

    context.m_project.removeModifier(elementId2, libTestUtils::TestRecordFeature::s_pathToInt2);
    context.m_project.process();

    EXPECT_FALSE(element1->isFailed());
    EXPECT_FALSE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element1->isInDependencyLoop());
    EXPECT_FALSE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());
}

// Check that one dependency loop does not prevent other elements from processing correctly.
TEST(ProjectTest, dependencyLoopAndProcessing) {
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    const babelwires::ElementId elementId1 =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId elementId2 =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId elementId3 =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());
    const babelwires::ElementId elementId4 =
        context.m_project.addFeatureElement(libTestUtils::TestFeatureElementData());

    const babelwires::FeatureElement* element1 = context.m_project.getFeatureElement(elementId1);
    const babelwires::FeatureElement* element2 = context.m_project.getFeatureElement(elementId2);
    const babelwires::FeatureElement* element3 = context.m_project.getFeatureElement(elementId3);
    const babelwires::FeatureElement* element4 = context.m_project.getFeatureElement(elementId4);

    context.m_project.process();

    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_sourceId = elementId2;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        context.m_project.addModifier(elementId1, modData);
    }
    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_sourceId = elementId4;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        context.m_project.addModifier(elementId3, modData);
    }

    context.m_project.process();

    EXPECT_FALSE(element1->isFailed());
    EXPECT_FALSE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element4->isFailed());
    EXPECT_FALSE(element1->isInDependencyLoop());
    EXPECT_FALSE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());
    EXPECT_FALSE(element4->isInDependencyLoop());

    {
        babelwires::AssignFromFeatureData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_sourceId = elementId1;
        modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        context.m_project.addModifier(elementId2, modData);
    }
    {
        babelwires::IntValueAssignmentData modData;
        modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
        modData.m_value = 16;
        context.m_project.addModifier(elementId4, modData);
    }

    context.m_project.process();

    EXPECT_TRUE(element1->isFailed());
    EXPECT_TRUE(element2->isFailed());
    EXPECT_FALSE(element3->isFailed());
    EXPECT_FALSE(element4->isFailed());
    EXPECT_TRUE(element1->isInDependencyLoop());
    EXPECT_TRUE(element2->isInDependencyLoop());
    EXPECT_FALSE(element3->isInDependencyLoop());
    EXPECT_FALSE(element4->isInDependencyLoop());

    ASSERT_NE(dynamic_cast<const libTestUtils::TestRecordFeature*>(element3->getOutputFeature()), nullptr);
    EXPECT_EQ(dynamic_cast<const libTestUtils::TestRecordFeature*>(element3->getOutputFeature())->m_intFeature2->get(),
              16);

    context.m_project.removeModifier(elementId2, libTestUtils::TestRecordFeature::s_pathToInt2);
    context.m_project.process();

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
    babelwires::FieldNameRegistryScope fieldNameRegistry;
    libTestUtils::TestProjectContext context;

    std::vector<babelwires::ElementId> idsToCheck0 = {libTestUtils::TestProjectData::c_sourceElementId,
                                                      libTestUtils::TestProjectData::c_processorId,
                                                      libTestUtils::TestProjectData::c_targetElementId};
    context.m_project.updateWithAvailableIds(idsToCheck0);
    EXPECT_EQ(idsToCheck0[0], libTestUtils::TestProjectData::c_sourceElementId);
    EXPECT_EQ(idsToCheck0[1], libTestUtils::TestProjectData::c_processorId);
    EXPECT_EQ(idsToCheck0[2], libTestUtils::TestProjectData::c_targetElementId);

    libTestUtils::TestProjectData projectData;
    context.m_project.setProjectData(projectData);

    std::vector<babelwires::ElementId> idsToCheck1 = {libTestUtils::TestProjectData::c_sourceElementId,
                                                      libTestUtils::TestProjectData::c_processorId,
                                                      libTestUtils::TestProjectData::c_targetElementId};
    context.m_project.updateWithAvailableIds(idsToCheck1);
    EXPECT_NE(idsToCheck1[0], libTestUtils::TestProjectData::c_sourceElementId);
    EXPECT_NE(idsToCheck1[1], libTestUtils::TestProjectData::c_processorId);
    EXPECT_NE(idsToCheck1[2], libTestUtils::TestProjectData::c_targetElementId);
    EXPECT_NE(idsToCheck1[0], idsToCheck1[1]);
    EXPECT_NE(idsToCheck1[1], idsToCheck1[2]);
    EXPECT_NE(idsToCheck1[2], idsToCheck1[0]);

    for (int i = 0; i < 3; ++i) {
        libTestUtils::TestFeatureElementData data;
        data.m_id = idsToCheck1[i];
        EXPECT_EQ(context.m_project.addFeatureElement(data), idsToCheck1[i]);
    }

    std::vector<babelwires::ElementId> idsToCheck2 = {libTestUtils::TestProjectData::c_sourceElementId,
                                                      libTestUtils::TestProjectData::c_processorId,
                                                      libTestUtils::TestProjectData::c_targetElementId};
    context.m_project.updateWithAvailableIds(idsToCheck2);
    EXPECT_NE(idsToCheck2[0], libTestUtils::TestProjectData::c_sourceElementId);
    EXPECT_NE(idsToCheck2[1], libTestUtils::TestProjectData::c_processorId);
    EXPECT_NE(idsToCheck2[2], libTestUtils::TestProjectData::c_targetElementId);
    EXPECT_NE(idsToCheck2[0], idsToCheck1[0]);
    EXPECT_NE(idsToCheck2[1], idsToCheck1[1]);
    EXPECT_NE(idsToCheck2[2], idsToCheck1[2]);
    EXPECT_NE(idsToCheck2[0], idsToCheck2[1]);
    EXPECT_NE(idsToCheck2[1], idsToCheck2[2]);
    EXPECT_NE(idsToCheck2[2], idsToCheck2[0]);
}
