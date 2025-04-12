#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>

#include <gtest/gtest.h>

#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Types/File/fileType.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>

testUtils::TestProjectData::TestProjectData()
    : m_sourceFilePath(std::string("testSourceFile") + testUtils::TestSourceFileFormat::getFileExtension())
    , m_targetFilePath(std::string("testTargetFile") + testUtils::TestSourceFileFormat::getFileExtension()) {
    m_projectId = 1243;
    {
        babelwires::TargetFileNodeData data;
        data.m_factoryIdentifier = testUtils::TestTargetFileFormat::getThisIdentifier();
        data.m_id = c_targetNodeId;
        data.m_filePath = m_targetFilePath;
        {
            babelwires::ConnectionModifierData modData;
            modData.m_targetPath = testUtils::getTestFileElementPathToInt0();
            modData.m_sourceId = c_processorId;
            modData.m_sourcePath = testUtils::TestProcessorInputOutputType::s_pathToArray_3;
            data.m_modifiers.emplace_back(modData.clone());
        }
        m_nodes.emplace_back(data.clone());
    }
    {
        babelwires::ProcessorNodeData data;
        data.m_factoryIdentifier = testUtils::TestProcessor::getFactoryIdentifier();
        data.m_id = c_processorId;
        {
            babelwires::ConnectionModifierData modData;
            modData.m_targetPath = testUtils::TestProcessorInputOutputType::s_pathToInt;
            modData.m_sourceId = c_sourceNodeId;
            modData.m_sourcePath = testUtils::getTestFileElementPathToInt0();
            data.m_modifiers.emplace_back(modData.clone());
        }
        {
            babelwires::ValueAssignmentData modData(babelwires::IntValue(44));
            modData.m_targetPath = testUtils::TestProcessorInputOutputType::s_pathToInt2;
            data.m_modifiers.emplace_back(modData.clone());
        }
        data.m_expandedPaths.emplace_back(testUtils::TestProcessorInputOutputType::s_pathToArray);
        m_nodes.emplace_back(data.clone());
    }
    {
        babelwires::SourceFileNodeData data;
        data.m_id = c_sourceNodeId;
        data.m_factoryIdentifier = testUtils::TestSourceFileFormat::getThisIdentifier();
        data.m_filePath = m_sourceFilePath;
        m_nodes.emplace_back(data.clone());
    }
}

void testUtils::TestProjectData::setFilePaths(std::string_view sourceFilePath, std::string_view targetFilePath) {
    assert(m_nodes.size() == 3);
    m_nodes[2]->as<babelwires::SourceFileNodeData>()->m_filePath = sourceFilePath;
    m_nodes[0]->as<babelwires::TargetFileNodeData>()->m_filePath = targetFilePath;
    m_sourceFilePath = sourceFilePath;
    m_targetFilePath = targetFilePath;
}

void testUtils::TestProjectData::testProjectDataAndDisciminators(
    const babelwires::ProjectData& projectData, int recordIntDiscriminator, int recordArrayDiscriminator,
    int recordRecordDiscriminator, int recordInt2Disciminator, int fileIntChildDiscriminator) {
    ASSERT_EQ(projectData.m_nodes.size(), 3);

    const babelwires::NodeData* sortedElements[3] = {projectData.m_nodes[0].get(), projectData.m_nodes[1].get(),
                                                     projectData.m_nodes[2].get()};
    std::sort(&sortedElements[0], &sortedElements[3],
              [](const babelwires::NodeData* a, const babelwires::NodeData* b) { return a->m_id < b->m_id; });

    EXPECT_EQ(sortedElements[0]->m_id, 6);
    ASSERT_EQ(sortedElements[0]->m_modifiers.size(), 2);
    {
        const babelwires::ModifierData* sortedModifiers[2] = {sortedElements[0]->m_modifiers[0].get(),
                                                              sortedElements[0]->m_modifiers[1].get()};
        std::sort(&sortedModifiers[0], &sortedModifiers[2],
                  [](const babelwires::ModifierData* a, const babelwires::ModifierData* b) {
                      return a->m_targetPath < b->m_targetPath;
                  });

        auto modData1 = sortedModifiers[0]->as<babelwires::ConnectionModifierData>();
        ASSERT_TRUE(modData1);
        EXPECT_EQ(*modData1->m_targetPath.getStep(0).asField(),
                  testUtils::TestProcessorInputOutputType::s_intIdInitializer);
        EXPECT_EQ(modData1->m_targetPath.getStep(0).asField()->getDiscriminator(), recordIntDiscriminator);
        ASSERT_EQ(modData1->m_sourcePath.getNumSteps(), 2);
        EXPECT_EQ(*modData1->m_sourcePath.getStep(0).asField(), babelwires::FileType::getStepToContents());
        EXPECT_EQ(*modData1->m_sourcePath.getStep(1).asField(), TestSimpleRecordType::s_int0IdInitializer);
        EXPECT_EQ(modData1->m_sourcePath.getStep(1).asField()->getDiscriminator(), fileIntChildDiscriminator);

        auto modData2 = sortedModifiers[1]->as<babelwires::ValueAssignmentData>();
        ASSERT_TRUE(modData2);
        ASSERT_GE(modData2->m_targetPath.getNumSteps(), 2);
        EXPECT_EQ(*modData2->m_targetPath.getStep(0).asField(),
                  testUtils::TestProcessorInputOutputType::s_recordIdInitializer);
        EXPECT_EQ(modData2->m_targetPath.getStep(0).asField()->getDiscriminator(), recordRecordDiscriminator);
        EXPECT_EQ(*modData2->m_targetPath.getStep(1).asField(), testUtils::TestSimpleRecordType::s_int0IdInitializer);
        EXPECT_EQ(modData2->m_targetPath.getStep(1).asField()->getDiscriminator(), recordInt2Disciminator);
    }

    ASSERT_EQ(sortedElements[0]->m_expandedPaths.size(), 1);
    EXPECT_EQ(sortedElements[0]->m_expandedPaths[0].getNumSteps(), 1);
    EXPECT_EQ(*sortedElements[0]->m_expandedPaths[0].getStep(0).asField(),
              testUtils::TestProcessorInputOutputType::s_arrayIdInitializer);
    EXPECT_EQ(sortedElements[0]->m_expandedPaths[0].getStep(0).asField()->getDiscriminator(), recordArrayDiscriminator);

    EXPECT_EQ(sortedElements[1]->m_id, 12);

    EXPECT_EQ(sortedElements[2]->m_id, 45);
    ASSERT_EQ(sortedElements[2]->m_modifiers.size(), 1);
    auto modData0 = sortedElements[2]->m_modifiers[0].get()->as<babelwires::ConnectionModifierData>();
    ASSERT_TRUE(modData0);
    ASSERT_EQ(modData0->m_targetPath.getNumSteps(), 2);
    EXPECT_EQ(*modData0->m_targetPath.getStep(0).asField(), babelwires::FileType::getStepToContents());
    EXPECT_EQ(*modData0->m_targetPath.getStep(1).asField(), TestSimpleRecordType::s_int0IdInitializer);
    EXPECT_EQ(modData0->m_targetPath.getStep(1).asField()->getDiscriminator(), fileIntChildDiscriminator);
    EXPECT_EQ(*modData0->m_sourcePath.getStep(0).asField(),
              testUtils::TestProcessorInputOutputType::s_arrayIdInitializer);
    EXPECT_EQ(modData0->m_sourcePath.getStep(0).asField()->getDiscriminator(), recordArrayDiscriminator);
    EXPECT_EQ(sortedElements[2]->m_expandedPaths.size(), 0);
}

void testUtils::TestProjectData::testProjectData(const babelwires::ProjectContext& context,
                                                 const babelwires::ProjectData& projectData) {
    testUtils::TestProjectData::testProjectDataAndDisciminators(
        projectData, TestProcessorInputOutputType::getIntId().getDiscriminator(),
        TestProcessorInputOutputType::getArrayId().getDiscriminator(),
        TestProcessorInputOutputType::getRecordId().getDiscriminator(),
        TestSimpleRecordType::getInt0Id().getDiscriminator(), TestSimpleRecordType::getInt0Id().getDiscriminator());
}

void testUtils::TestProjectData::resolvePathsInCurrentContext(const babelwires::ProjectContext& context) {
    babelwires::ValueTreeRoot testRecord(context.m_typeSystem, testUtils::TestProcessorInputOutputType::getThisType());
    testRecord.setToDefault();
    babelwires::ValueTreeRoot testFileFeature(context.m_typeSystem, testUtils::getTestFileType());

    // These have side-effects on the mutable field discriminators in the paths.
    auto modData0 = m_nodes[0]->m_modifiers[0].get()->as<babelwires::ConnectionModifierData>();
    tryFollowPath(modData0->m_targetPath, testFileFeature);
    tryFollowPath(modData0->m_sourcePath, testRecord);
    auto modData1 = m_nodes[1]->m_modifiers[0].get()->as<babelwires::ConnectionModifierData>();
    tryFollowPath(modData1->m_targetPath, testRecord);
    tryFollowPath(modData1->m_sourcePath, testFileFeature);
    auto modData2 = m_nodes[1]->m_modifiers[1].get()->as<babelwires::ValueAssignmentData>();
    tryFollowPath(modData2->m_targetPath, testRecord);
    tryFollowPath(m_nodes[1]->m_expandedPaths[0], testRecord);
}
