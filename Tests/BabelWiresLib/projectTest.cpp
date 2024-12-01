#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNode.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testArrayType.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecordType.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

#include <fstream>

TEST(ProjectTest, setAndExtractProjectData) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData projectData;

    // Make real data for project.
    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(babelwires::pathToString(sourceFilePath.m_filePath),
                             babelwires::pathToString(targetFilePath.m_filePath));
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

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    EXPECT_NE(node, nullptr);
    EXPECT_NE(node->as<babelwires::ValueNode>(), nullptr);
    EXPECT_TRUE(node->isChanged(babelwires::Node::Changes::NodeIsNew));

    testEnvironment.m_project.removeNode(elementId);

    const babelwires::Node* node2 = testEnvironment.m_project.getNode(elementId);
    EXPECT_EQ(node2, nullptr);

    const auto& removedElements = testEnvironment.m_project.getRemovedNodes();

    EXPECT_EQ(removedElements.size(), 1);
    const auto it = removedElements.find(elementId);
    ASSERT_NE(it, removedElements.end());
    EXPECT_EQ(it->first, elementId);
    EXPECT_EQ(it->second.get(), node);

    testEnvironment.m_project.clearChanges();

    EXPECT_EQ(testEnvironment.m_project.getRemovedNodes().size(), 0);
}

TEST(ProjectTest, addAndRemoveLocalModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    const babelwires::Path pathToFeature = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
    EXPECT_EQ(node->findModifier(pathToFeature), nullptr);

    babelwires::ValueAssignmentData modData(babelwires::IntValue(199));
    modData.m_targetPath = pathToFeature;

    testEnvironment.m_project.addModifier(elementId, modData);

    const babelwires::Modifier* const modifier = node->findModifier(pathToFeature);
    EXPECT_NE(modifier, nullptr);
    EXPECT_TRUE(modifier->isChanged(babelwires::Modifier::Changes::ModifierIsNew));

    testEnvironment.m_project.removeModifier(elementId, pathToFeature);
    EXPECT_EQ(node->findModifier(pathToFeature), nullptr);
}

TEST(ProjectTest, addAndRemoveConnectionModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId sourceNodeId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());
    const babelwires::Node* sourceElement = testEnvironment.m_project.getNode(sourceNodeId);
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::NodeId targetNodeId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());
    const babelwires::Node* targetElement = testEnvironment.m_project.getNode(targetNodeId);
    ASSERT_NE(targetElement, nullptr);

    const babelwires::Path pathToTargetFeature = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
    const babelwires::Path pathToSourceFeature = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(0);

    EXPECT_EQ(sourceElement->findModifier(pathToTargetFeature), nullptr);
    babelwires::ConnectionModifierData modData;
    modData.m_targetPath = pathToTargetFeature;
    modData.m_sourceId = sourceNodeId;
    modData.m_sourcePath = pathToSourceFeature;

    testEnvironment.m_project.addModifier(targetNodeId, modData);

    const babelwires::Modifier* const modifier = targetElement->findModifier(pathToTargetFeature);
    EXPECT_NE(modifier, nullptr);
    EXPECT_TRUE(modifier->isChanged(babelwires::Modifier::Changes::ModifierIsNew));

    testEnvironment.m_project.removeModifier(targetNodeId, pathToTargetFeature);
    EXPECT_EQ(sourceElement->findModifier(pathToTargetFeature), nullptr);
}

TEST(ProjectTest, addAndRemoveArrayEntriesSimple) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    const babelwires::Path pathToArray = testUtils::TestComplexRecordElementData::getPathToRecordArray();
    EXPECT_EQ(node->findModifier(pathToArray), nullptr);

    testEnvironment.m_project.process();
    // You can add at the end.
    testEnvironment.m_project.addArrayEntries(elementId, pathToArray, 2, 2, true);
    {
        const babelwires::Modifier* const modifier = node->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        ASSERT_TRUE(modifier->getModifierData().as<babelwires::ArraySizeModifierData>());
        EXPECT_EQ(static_cast<const babelwires::ArraySizeModifierData&>(modifier->getModifierData()).m_size,
                  testUtils::TestSimpleArrayType::s_defaultSize + 2);
    }

    testEnvironment.m_project.process();
    testEnvironment.m_project.addArrayEntries(elementId, pathToArray, 1, 2, true);
    {
        const babelwires::Modifier* const modifier = node->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        EXPECT_EQ(modifier->getModifierData().as<babelwires::ArraySizeModifierData>()->m_size,
                  testUtils::TestSimpleArrayType::s_defaultSize + 4);
    }

    testEnvironment.m_project.process();
    testEnvironment.m_project.removeArrayEntries(elementId, pathToArray, 2, 2, true);
    {
        const babelwires::Modifier* const modifier = node->findModifier(pathToArray);
        ASSERT_NE(modifier, nullptr);
        EXPECT_EQ(modifier->getModifierData().as<babelwires::ArraySizeModifierData>()->m_size,
                  testUtils::TestSimpleArrayType::s_defaultSize + 2);
    }

    testEnvironment.m_project.process();
    // The false means the modifier will be removed.
    testEnvironment.m_project.removeArrayEntries(elementId, pathToArray, 2, 2, false);
    EXPECT_EQ(node->findModifier(pathToArray), nullptr);
}

namespace {
    // Adapt tests to slightly changed API.
    void addArrayEntries(babelwires::Project& project, babelwires::NodeId elementId,
                         const babelwires::Path& pathToArray, int indexOfNewElement, int numEntriesToAdd,
                         bool ensureModifier) {
        project.addArrayEntries(elementId, pathToArray, indexOfNewElement, numEntriesToAdd, ensureModifier);
        project.adjustModifiersInArrayElements(elementId, pathToArray, indexOfNewElement, numEntriesToAdd);
    };

    void removeArrayEntries(babelwires::Project& project, babelwires::NodeId elementId,
                            const babelwires::Path& pathToArray, int indexOfElementToRemove, int numEntriesToRemove,
                            bool ensureModifier) {
        project.removeArrayEntries(elementId, pathToArray, indexOfElementToRemove, numEntriesToRemove, ensureModifier);
        project.adjustModifiersInArrayElements(elementId, pathToArray, indexOfElementToRemove, -numEntriesToRemove);
    }
} // namespace

TEST(ProjectTest, addAndRemoveArrayEntriesModifier) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    const babelwires::Path pathToArray = testUtils::TestComplexRecordElementData::getPathToRecordArray();
    const babelwires::Path pathToArray1 = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
    const babelwires::Path pathToArray2 = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(2);
    const babelwires::Path pathToArray4 = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(4);

    babelwires::ValueAssignmentData modData(babelwires::IntValue(702));
    modData.m_targetPath = pathToArray1;
    testEnvironment.m_project.addModifier(elementId, modData);

    EXPECT_EQ(node->findModifier(pathToArray), nullptr);

    testEnvironment.m_project.process();

    // Add after.
    addArrayEntries(testEnvironment.m_project, elementId, pathToArray, 2, 2, true);
    EXPECT_NE(node->findModifier(pathToArray1), nullptr);

    testEnvironment.m_project.process();

    // Add before.
    addArrayEntries(testEnvironment.m_project, elementId, pathToArray, 1, 3, true);
    EXPECT_EQ(node->findModifier(pathToArray1), nullptr);
    EXPECT_NE(node->findModifier(pathToArray4), nullptr);

    testEnvironment.m_project.process();

    // Remove after
    removeArrayEntries(testEnvironment.m_project, elementId, pathToArray, 5, 1, true);
    EXPECT_NE(node->findModifier(pathToArray4), nullptr);

    // Remove before.
    testEnvironment.m_project.process();
    removeArrayEntries(testEnvironment.m_project, elementId, pathToArray, 0, 2, true);
    EXPECT_NE(node->findModifier(pathToArray2), nullptr);

    // We don't test removal of modifiers, because responsibility for doing this is left to the
    // commands. (In fact, add/removeArrayEntries already does more than it should.)
}

TEST(ProjectTest, addAndRemoveArrayEntriesSource) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId sourceNodeId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());
    const babelwires::Node* sourceElement = testEnvironment.m_project.getNode(sourceNodeId);
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::NodeId targetNodeId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());
    const babelwires::Node* targetElement = testEnvironment.m_project.getNode(targetNodeId);
    ASSERT_NE(targetElement, nullptr);

    const babelwires::Path pathToTargetFeature = testUtils::TestComplexRecordElementData::getPathToRecordInt0();
    const babelwires::Path pathToArray = testUtils::TestComplexRecordElementData::getPathToRecordArray();
    const babelwires::Path pathToArray1 = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(1);
    const babelwires::Path pathToArray2 = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(2);
    const babelwires::Path pathToArray4 = testUtils::TestComplexRecordElementData::getPathToRecordArrayEntry(4);

    EXPECT_EQ(sourceElement->findModifier(pathToArray), nullptr);

    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = pathToTargetFeature;
        modData.m_sourceId = sourceNodeId;
        modData.m_sourcePath = pathToArray1;
        testEnvironment.m_project.addModifier(targetNodeId, modData);
    }
    const babelwires::ConnectionModifierData& connectionData =
        *targetElement->findModifier(pathToTargetFeature)->getModifierData().as<babelwires::ConnectionModifierData>();

    testEnvironment.m_project.process();

    // The new elements are after the modifier, so it should be in the same place.
    addArrayEntries(testEnvironment.m_project, sourceNodeId, pathToArray, 2, 2, true);
    EXPECT_EQ(connectionData.m_sourcePath, pathToArray1);

    testEnvironment.m_project.process();

    // The new elements should mean the modifier moved.
    addArrayEntries(testEnvironment.m_project, sourceNodeId, pathToArray, 1, 3, true);
    EXPECT_EQ(connectionData.m_sourcePath, pathToArray4);

    testEnvironment.m_project.process();

    // Remove after
    removeArrayEntries(testEnvironment.m_project, sourceNodeId, pathToArray, 5, 1, true);
    EXPECT_EQ(connectionData.m_sourcePath, pathToArray4);

    // Remove before.
    testEnvironment.m_project.process();
    removeArrayEntries(testEnvironment.m_project, sourceNodeId, pathToArray, 0, 2, true);
    EXPECT_EQ(connectionData.m_sourcePath, pathToArray2);

    // We don't test removal of modifiers, because responsibility for doing this is left to the
    // commands. (In fact, add/removeArrayEntries already does more than it should.)
}

TEST(ProjectTest, uiProperties) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData testFeatureData;
    testFeatureData.m_uiData.m_uiPosition.m_x = 8;
    testFeatureData.m_uiData.m_uiPosition.m_y = 2;
    testFeatureData.m_uiData.m_uiSize.m_width = 77;

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(testFeatureData);

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    EXPECT_EQ(node->getUiPosition().m_x, 8);
    EXPECT_EQ(node->getUiPosition().m_y, 2);
    EXPECT_EQ(node->getUiSize().m_width, 77);
    testEnvironment.m_project.setNodePosition(elementId, babelwires::UiPosition{10, 12});

    EXPECT_EQ(node->getUiPosition().m_x, 10);
    EXPECT_EQ(node->getUiPosition().m_y, 12);
    EXPECT_EQ(node->getUiSize().m_width, 77);

    testEnvironment.m_project.setNodeContentsSize(elementId, babelwires::UiSize{66});

    EXPECT_EQ(node->getUiPosition().m_x, 10);
    EXPECT_EQ(node->getUiPosition().m_y, 12);
    EXPECT_EQ(node->getUiSize().m_width, 66);
}

TEST(ProjectTest, elementIds) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::NodeId elementId =
        testEnvironment.m_project.addNode(testUtils::TestComplexRecordElementData());
    EXPECT_NE(elementId, babelwires::INVALID_NODE_ID);
    ASSERT_NE(elementId, 3);

    const babelwires::Node* node = testEnvironment.m_project.getNode(elementId);
    ASSERT_NE(node, nullptr);

    testUtils::TestComplexRecordElementData elementData;
    elementData.m_id = 3;

    const babelwires::NodeId nodeId1 = testEnvironment.m_project.addNode(elementData);
    EXPECT_EQ(nodeId1, 3);

    const babelwires::Node* node1 = testEnvironment.m_project.getNode(nodeId1);
    ASSERT_NE(node1, nullptr);
    EXPECT_EQ(node1->getNodeData().m_id, nodeId1);
    EXPECT_NE(node, node1);

    const babelwires::NodeId nodeId2 = testEnvironment.m_project.addNode(elementData);
    EXPECT_NE(nodeId2, babelwires::INVALID_NODE_ID);
    EXPECT_NE(nodeId2, nodeId1);

    const babelwires::Node* node2 = testEnvironment.m_project.getNode(nodeId2);
    EXPECT_NE(node2, nullptr);
    EXPECT_NE(node1, node2);
}

TEST(ProjectTest, reloadSource) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TempFilePath tempFilePath("testSource." + testUtils::TestSourceFileFormat::getFileExtension());

    testUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 14);

    babelwires::SourceFileNodeData sourceFileData;
    sourceFileData.m_filePath = tempFilePath;
    sourceFileData.m_factoryIdentifier = testUtils::TestSourceFileFormat::getThisIdentifier();

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(sourceFileData);
    const babelwires::Node* node =
        testEnvironment.m_project.getNode(elementId)->as<babelwires::Node>();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->getOutput(), nullptr);

    auto getIntInElement = [node]() {
        testUtils::TestSimpleRecordType::ConstInstance instance(node->getOutput()
                                                                    ->is<babelwires::ValueTreeNode>()
                                                                    .getChild(0)
                                                                    ->is<babelwires::ValueTreeNode>());
        return instance.getintR0().get();
    };

    EXPECT_EQ(getIntInElement(), 14);

    testUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 88);

    testEnvironment.m_project.tryToReloadSource(elementId);
    EXPECT_EQ(getIntInElement(), 88);

    testUtils::TestSourceFileFormat::writeToTestFile(tempFilePath, 55);

    testEnvironment.m_project.tryToReloadAllSources();
    EXPECT_EQ(getIntInElement(), 55);

    tempFilePath.tryRemoveFile();

    testEnvironment.m_log.clear();
    testEnvironment.m_project.tryToReloadSource(elementId);
    EXPECT_TRUE(node->isFailed());
    testEnvironment.m_log.hasSubstringIgnoreCase("could not be loaded");
}

TEST(ProjectTest, saveTarget) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TempFilePath tempFilePath("testTarget." + testUtils::TestSourceFileFormat::getFileExtension());

    babelwires::TargetFileNodeData targetFileData;
    targetFileData.m_filePath = tempFilePath;
    targetFileData.m_factoryIdentifier = testUtils::TestTargetFileFormat::getThisIdentifier();

    const babelwires::NodeId elementId = testEnvironment.m_project.addNode(targetFileData);
    babelwires::Node* node =
        testEnvironment.m_project.getNode(elementId)->as<babelwires::Node>();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->getInput(), nullptr);

    testUtils::TestSimpleRecordType::Instance instance(node->getInputNonConst(babelwires::Path())
                                                           ->is<babelwires::ValueTreeNode>()
                                                           .getChild(0)
                                                           ->is<babelwires::ValueTreeNode>());

    instance.getintR0().set(47);

    testEnvironment.m_project.tryToSaveTarget(elementId);

    EXPECT_EQ(testUtils::TestSourceFileFormat::getFileData(tempFilePath), 47);

    instance.getintR0().set(30);
    testEnvironment.m_project.tryToSaveTarget(elementId);
    EXPECT_EQ(testUtils::TestSourceFileFormat::getFileData(tempFilePath), 30);

    instance.getintR0().set(79);
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
    projectData.setFilePaths(babelwires::pathToString(sourceFilePath.m_filePath),
                             babelwires::pathToString(targetFilePath.m_filePath));
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    testEnvironment.m_project.setProjectData(projectData);
    testEnvironment.m_project.process();

    const babelwires::Node* sourceElement =
        testEnvironment.m_project.getNode(testUtils::TestProjectData::c_sourceNodeId);
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::Node* processor =
        testEnvironment.m_project.getNode(testUtils::TestProjectData::c_processorId);
    ASSERT_NE(processor, nullptr);
    const babelwires::ValueTreeNode* processorInput = processor->getInput();
    ASSERT_NE(processorInput, nullptr);
    const babelwires::ValueTreeNode* processorOutput = processor->getOutput();
    ASSERT_NE(processorOutput, nullptr);

    const babelwires::Node* targetElement =
        testEnvironment.m_project.getNode(testUtils::TestProjectData::c_targetNodeId);
    ASSERT_NE(targetElement, nullptr);

    testUtils::TestSimpleRecordType::ConstInstance targetInput(*targetElement->getInput()->getChild(0));
    // 4rd array entry, where they count up from the input value (3).
    EXPECT_EQ(targetInput.getintR0().get(), 6);

    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 4);
    testEnvironment.m_project.tryToReloadSource(testUtils::TestProjectData::c_sourceNodeId);
    testEnvironment.m_project.process();
    EXPECT_EQ(targetInput.getintR0().get(), 7);

    // Removing this modifier will mean the output array is shorter than the modifier at the target requires.
    testEnvironment.m_project.removeModifier(testUtils::TestProjectData::c_processorId,
                                             testUtils::TestProcessorInputOutputType::s_pathToInt);
    testEnvironment.m_project.process();
    EXPECT_TRUE(targetElement->findModifier(testUtils::getTestFileElementPathToInt0())->isFailed());
    EXPECT_EQ(targetInput.getintR0().get(), 0);

    babelwires::NodeId newProcId;
    {
        babelwires::ProcessorNodeData procData;
        procData.m_factoryIdentifier = testUtils::TestProcessor::getFactoryIdentifier();

        babelwires::ValueAssignmentData modData(babelwires::IntValue(5));
        modData.m_targetPath = testUtils::TestProcessorInputOutputType::s_pathToInt2;
        procData.m_modifiers.emplace_back(modData.clone());

        newProcId = testEnvironment.m_project.addNode(procData);
    }

    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = testUtils::TestProcessorInputOutputType::s_pathToInt;
        modData.m_sourceId = newProcId;
        modData.m_sourcePath = testUtils::TestProcessorInputOutputType::s_pathToInt2;
        testEnvironment.m_project.addModifier(testUtils::TestProjectData::c_processorId, modData);
    }
    testEnvironment.m_project.process();
    EXPECT_EQ(targetInput.getintR0().get(), 8);
}

TEST(ProjectTest, dependencyLoop) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;

    const babelwires::NodeId nodeId1 = testEnvironment.m_project.addNode(elementData);
    const babelwires::NodeId nodeId2 = testEnvironment.m_project.addNode(elementData);
    const babelwires::NodeId nodeId3 = testEnvironment.m_project.addNode(elementData);

    const babelwires::Node* node1 = testEnvironment.m_project.getNode(nodeId1);
    const babelwires::Node* node2 = testEnvironment.m_project.getNode(nodeId2);
    const babelwires::Node* node3 = testEnvironment.m_project.getNode(nodeId3);

    testEnvironment.m_project.process();

    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = elementData.getPathToRecordInt0();
        modData.m_sourceId = nodeId2;
        modData.m_sourcePath = elementData.getPathToRecordInt0();
        testEnvironment.m_project.addModifier(nodeId1, modData);
    }
    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = elementData.getPathToRecordInt0();
        modData.m_sourceId = nodeId3;
        modData.m_sourcePath = elementData.getPathToRecordInt0();
        testEnvironment.m_project.addModifier(nodeId2, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_FALSE(node1->isFailed());
    EXPECT_FALSE(node2->isFailed());
    EXPECT_FALSE(node3->isFailed());
    EXPECT_FALSE(node1->isInDependencyLoop());
    EXPECT_FALSE(node2->isInDependencyLoop());
    EXPECT_FALSE(node3->isInDependencyLoop());

    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = elementData.getPathToRecordInt0();
        modData.m_sourceId = nodeId1;
        modData.m_sourcePath = elementData.getPathToRecordInt0();
        testEnvironment.m_project.addModifier(nodeId3, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_TRUE(node1->isFailed());
    EXPECT_TRUE(node2->isFailed());
    EXPECT_TRUE(node3->isFailed());
    EXPECT_TRUE(node1->isInDependencyLoop());
    EXPECT_TRUE(node2->isInDependencyLoop());
    EXPECT_TRUE(node3->isInDependencyLoop());

    testEnvironment.m_project.removeModifier(nodeId2, elementData.getPathToRecordInt0());
    testEnvironment.m_project.process();

    EXPECT_FALSE(node1->isFailed());
    EXPECT_FALSE(node2->isFailed());
    EXPECT_FALSE(node3->isFailed());
    EXPECT_FALSE(node1->isInDependencyLoop());
    EXPECT_FALSE(node2->isInDependencyLoop());
    EXPECT_FALSE(node3->isInDependencyLoop());
}

// Check that one dependency loop does not prevent other elements from processing correctly.
TEST(ProjectTest, dependencyLoopAndProcessing) {
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestComplexRecordElementData elementData;

    const babelwires::NodeId nodeId1 = testEnvironment.m_project.addNode(elementData);
    const babelwires::NodeId nodeId2 = testEnvironment.m_project.addNode(elementData);
    const babelwires::NodeId nodeId3 = testEnvironment.m_project.addNode(elementData);
    const babelwires::NodeId elementId4 = testEnvironment.m_project.addNode(elementData);

    const babelwires::Node* node1 = testEnvironment.m_project.getNode(nodeId1);
    const babelwires::Node* node2 = testEnvironment.m_project.getNode(nodeId2);
    const babelwires::Node* node3 = testEnvironment.m_project.getNode(nodeId3);
    const babelwires::Node* node4 = testEnvironment.m_project.getNode(elementId4);

    testEnvironment.m_project.process();

    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = elementData.getPathToRecordInt0();
        modData.m_sourceId = nodeId2;
        modData.m_sourcePath = elementData.getPathToRecordInt0();
        testEnvironment.m_project.addModifier(nodeId1, modData);
    }
    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = elementData.getPathToRecordInt0();
        modData.m_sourceId = elementId4;
        modData.m_sourcePath = elementData.getPathToRecordInt0();
        testEnvironment.m_project.addModifier(nodeId3, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_FALSE(node1->isFailed());
    EXPECT_FALSE(node2->isFailed());
    EXPECT_FALSE(node3->isFailed());
    EXPECT_FALSE(node4->isFailed());
    EXPECT_FALSE(node1->isInDependencyLoop());
    EXPECT_FALSE(node2->isInDependencyLoop());
    EXPECT_FALSE(node3->isInDependencyLoop());
    EXPECT_FALSE(node4->isInDependencyLoop());

    {
        babelwires::ConnectionModifierData modData;
        modData.m_targetPath = elementData.getPathToRecordInt0();
        modData.m_sourceId = nodeId1;
        modData.m_sourcePath = elementData.getPathToRecordInt0();
        testEnvironment.m_project.addModifier(nodeId2, modData);
    }
    {
        babelwires::ValueAssignmentData modData(babelwires::IntValue(16));
        modData.m_targetPath = elementData.getPathToRecordInt0();
        testEnvironment.m_project.addModifier(elementId4, modData);
    }

    testEnvironment.m_project.process();

    EXPECT_TRUE(node1->isFailed());
    EXPECT_TRUE(node2->isFailed());
    EXPECT_FALSE(node3->isFailed());
    EXPECT_FALSE(node4->isFailed());
    EXPECT_TRUE(node1->isInDependencyLoop());
    EXPECT_TRUE(node2->isInDependencyLoop());
    EXPECT_FALSE(node3->isInDependencyLoop());
    EXPECT_FALSE(node4->isInDependencyLoop());

    ASSERT_NE(node3->getOutput(), nullptr);
    testUtils::TestComplexRecordType::ConstInstance instance(*node3->getOutput());
    EXPECT_EQ(instance.getintR0().get(), 16);

    testEnvironment.m_project.removeModifier(nodeId2, elementData.getPathToRecordInt0());
    testEnvironment.m_project.process();

    EXPECT_FALSE(node1->isFailed());
    EXPECT_FALSE(node2->isFailed());
    EXPECT_FALSE(node3->isFailed());
    EXPECT_FALSE(node4->isFailed());
    EXPECT_FALSE(node1->isInDependencyLoop());
    EXPECT_FALSE(node2->isInDependencyLoop());
    EXPECT_FALSE(node3->isInDependencyLoop());
    EXPECT_FALSE(node4->isInDependencyLoop());
}

TEST(ProjectTest, updateWithAvailableIds) {
    testUtils::TestEnvironment testEnvironment;

    std::vector<babelwires::NodeId> idsToCheck0 = {testUtils::TestProjectData::c_sourceNodeId,
                                                      testUtils::TestProjectData::c_processorId,
                                                      testUtils::TestProjectData::c_targetNodeId};
    testEnvironment.m_project.updateWithAvailableIds(idsToCheck0);
    EXPECT_EQ(idsToCheck0[0], testUtils::TestProjectData::c_sourceNodeId);
    EXPECT_EQ(idsToCheck0[1], testUtils::TestProjectData::c_processorId);
    EXPECT_EQ(idsToCheck0[2], testUtils::TestProjectData::c_targetNodeId);

    testUtils::TestProjectData projectData;
    testEnvironment.m_project.setProjectData(projectData);

    std::vector<babelwires::NodeId> idsToCheck1 = {testUtils::TestProjectData::c_sourceNodeId,
                                                      testUtils::TestProjectData::c_processorId,
                                                      testUtils::TestProjectData::c_targetNodeId};
    testEnvironment.m_project.updateWithAvailableIds(idsToCheck1);
    EXPECT_NE(idsToCheck1[0], testUtils::TestProjectData::c_sourceNodeId);
    EXPECT_NE(idsToCheck1[1], testUtils::TestProjectData::c_processorId);
    EXPECT_NE(idsToCheck1[2], testUtils::TestProjectData::c_targetNodeId);
    EXPECT_NE(idsToCheck1[0], idsToCheck1[1]);
    EXPECT_NE(idsToCheck1[1], idsToCheck1[2]);
    EXPECT_NE(idsToCheck1[2], idsToCheck1[0]);

    for (int i = 0; i < 3; ++i) {
        testUtils::TestComplexRecordElementData data;
        data.m_id = idsToCheck1[i];
        EXPECT_EQ(testEnvironment.m_project.addNode(data), idsToCheck1[i]);
    }

    std::vector<babelwires::NodeId> idsToCheck2 = {testUtils::TestProjectData::c_sourceNodeId,
                                                      testUtils::TestProjectData::c_processorId,
                                                      testUtils::TestProjectData::c_targetNodeId};
    testEnvironment.m_project.updateWithAvailableIds(idsToCheck2);
    EXPECT_NE(idsToCheck2[0], testUtils::TestProjectData::c_sourceNodeId);
    EXPECT_NE(idsToCheck2[1], testUtils::TestProjectData::c_processorId);
    EXPECT_NE(idsToCheck2[2], testUtils::TestProjectData::c_targetNodeId);
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
    // Ensure this node fails completely.
    projectData.m_nodes[1]->m_factoryIdentifier = "flerg";

    // Make real data for project.
    testUtils::TempFilePath sourceFilePath(projectData.m_sourceFilePath);
    testUtils::TempFilePath targetFilePath(projectData.m_targetFilePath);
    projectData.setFilePaths(babelwires::pathToString(sourceFilePath.m_filePath),
                             babelwires::pathToString(targetFilePath.m_filePath));
    testUtils::TestSourceFileFormat::writeToTestFile(sourceFilePath, 3);

    testEnvironment.m_project.setProjectData(projectData);
    testEnvironment.m_project.process();

    const babelwires::Node* sourceElement =
        testEnvironment.m_project.getNode(testUtils::TestProjectData::c_sourceNodeId);
    ASSERT_NE(sourceElement, nullptr);

    const babelwires::Node* processor =
        testEnvironment.m_project.getNode(testUtils::TestProjectData::c_processorId);
    ASSERT_NE(processor, nullptr);
    ASSERT_TRUE(processor->isFailed());

    const babelwires::Node* targetElement =
        testEnvironment.m_project.getNode(testUtils::TestProjectData::c_targetNodeId);
    ASSERT_NE(targetElement, nullptr);
}
