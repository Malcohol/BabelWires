#include "Tests/BabelWiresLib/TestUtils/testProjectData.hpp"

#include <gtest/gtest.h>

#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/processorElementData.hpp"
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifierData.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProcessor.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"

libTestUtils::TestProjectData::TestProjectData()
    : m_sourceFilePath(std::string("testSourceFile") + libTestUtils::TestSourceFileFormat::getFileExtension())
    , m_targetFilePath(std::string("testTargetFile") + libTestUtils::TestSourceFileFormat::getFileExtension()) {
    m_projectId = 1243;
    {
        babelwires::TargetFileElementData data;
        data.m_factoryIdentifier = libTestUtils::TestTargetFileFormat::getThisIdentifier();
        data.m_id = c_targetElementId;
        data.m_filePath = m_targetFilePath;
        {
            babelwires::ConnectionModifierData modData;
            modData.m_pathToFeature = libTestUtils::TestFileFeature::s_pathToIntChild;
            modData.m_sourceId = c_processorId;
            modData.m_pathToSourceFeature = libTestUtils::TestRecordFeature::s_pathToArray_3;
            data.m_modifiers.emplace_back(modData.clone());
        }
        m_elements.emplace_back(data.clone());
    }
    {
        babelwires::ProcessorElementData data;
        data.m_factoryIdentifier = libTestUtils::TestProcessorFactory::getThisIdentifier();
        data.m_id = c_processorId;
        {
            babelwires::ConnectionModifierData modData;
            modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt;
            modData.m_sourceId = c_sourceElementId;
            modData.m_pathToSourceFeature = libTestUtils::TestFileFeature::s_pathToIntChild;
            data.m_modifiers.emplace_back(modData.clone());
        }
        {
            babelwires::IntValueAssignmentData modData;
            modData.m_pathToFeature = libTestUtils::TestRecordFeature::s_pathToInt2;
            modData.m_value = 44;
            data.m_modifiers.emplace_back(modData.clone());
        }
        data.m_expandedPaths.emplace_back(libTestUtils::TestRecordFeature::s_pathToArray);
        m_elements.emplace_back(data.clone());
    }
    {
        babelwires::SourceFileElementData data;
        data.m_id = c_sourceElementId;
        data.m_factoryIdentifier = libTestUtils::TestSourceFileFormat::getThisIdentifier();
        data.m_filePath = m_sourceFilePath;
        m_elements.emplace_back(data.clone());
    }
}

void libTestUtils::TestProjectData::setFilePaths(std::string_view sourceFilePath, std::string_view targetFilePath) {
    assert(m_elements.size() == 3);
    m_elements[2]->as<babelwires::SourceFileElementData>()->m_filePath = sourceFilePath;
    m_elements[0]->as<babelwires::TargetFileElementData>()->m_filePath = targetFilePath;
    m_sourceFilePath = sourceFilePath;
    m_targetFilePath = targetFilePath;
}

void libTestUtils::TestProjectData::testProjectDataAndDisciminators(
    const babelwires::ProjectData& projectData, int recordIntDiscriminator, int recordArrayDiscriminator,
    int recordRecordDiscriminator, int recordInt2Disciminator, int fileIntChildDiscriminator) {
    ASSERT_EQ(projectData.m_elements.size(), 3);

    const babelwires::ElementData* sortedElements[3] = {
        projectData.m_elements[0].get(), projectData.m_elements[1].get(), projectData.m_elements[2].get()};
    std::sort(&sortedElements[0], &sortedElements[3],
              [](const babelwires::ElementData* a, const babelwires::ElementData* b) { return a->m_id < b->m_id; });

    EXPECT_EQ(sortedElements[0]->m_id, 6);
    ASSERT_EQ(sortedElements[0]->m_modifiers.size(), 2);
    {
        const babelwires::ModifierData* sortedModifiers[2] = {sortedElements[0]->m_modifiers[0].get(),
                                                              sortedElements[0]->m_modifiers[1].get()};
        std::sort(&sortedModifiers[0], &sortedModifiers[2],
                  [](const babelwires::ModifierData* a, const babelwires::ModifierData* b) {
                      return a->m_pathToFeature < b->m_pathToFeature;
                  });

        auto modData1 = sortedModifiers[0]->as<babelwires::ConnectionModifierData>();
        ASSERT_TRUE(modData1);
        EXPECT_EQ(*modData1->m_pathToFeature.getStep(0).asField(), libTestUtils::TestRecordFeature::s_intIdInitializer);
        EXPECT_EQ(modData1->m_pathToFeature.getStep(0).asField()->getDiscriminator(), recordIntDiscriminator);
        EXPECT_EQ(*modData1->m_pathToSourceFeature.getStep(0).asField(),
                  libTestUtils::TestFileFeature::s_intChildInitializer);
        EXPECT_EQ(modData1->m_pathToSourceFeature.getStep(0).asField()->getDiscriminator(), fileIntChildDiscriminator);

        auto modData2 = sortedModifiers[1]->as<babelwires::IntValueAssignmentData>();
        ASSERT_TRUE(modData2);
        ASSERT_GE(modData2->m_pathToFeature.getNumSteps(), 2);
        EXPECT_EQ(*modData2->m_pathToFeature.getStep(0).asField(),
                  libTestUtils::TestRecordFeature::s_recordIdInitializer);
        EXPECT_EQ(modData2->m_pathToFeature.getStep(0).asField()->getDiscriminator(), recordRecordDiscriminator);
        EXPECT_EQ(*modData2->m_pathToFeature.getStep(1).asField(),
                  libTestUtils::TestRecordFeature::s_int2IdInitializer);
        EXPECT_EQ(modData2->m_pathToFeature.getStep(1).asField()->getDiscriminator(), recordInt2Disciminator);
    }

    ASSERT_EQ(sortedElements[0]->m_expandedPaths.size(), 1);
    EXPECT_EQ(sortedElements[0]->m_expandedPaths[0].getNumSteps(), 1);
    EXPECT_EQ(*sortedElements[0]->m_expandedPaths[0].getStep(0).asField(),
              libTestUtils::TestRecordFeature::s_arrayIdInitializer);
    EXPECT_EQ(sortedElements[0]->m_expandedPaths[0].getStep(0).asField()->getDiscriminator(), recordArrayDiscriminator);

    EXPECT_EQ(sortedElements[1]->m_id, 12);

    EXPECT_EQ(sortedElements[2]->m_id, 45);
    ASSERT_EQ(sortedElements[2]->m_modifiers.size(), 1);
    auto modData0 = sortedElements[2]->m_modifiers[0].get()->as<babelwires::ConnectionModifierData>();
    ASSERT_TRUE(modData0);
    EXPECT_EQ(*modData0->m_pathToFeature.getStep(0).asField(), libTestUtils::TestFileFeature::s_intChildInitializer);
    EXPECT_EQ(modData0->m_pathToFeature.getStep(0).asField()->getDiscriminator(), fileIntChildDiscriminator);
    EXPECT_EQ(*modData0->m_pathToSourceFeature.getStep(0).asField(),
              libTestUtils::TestRecordFeature::s_arrayIdInitializer);
    EXPECT_EQ(modData0->m_pathToSourceFeature.getStep(0).asField()->getDiscriminator(), recordArrayDiscriminator);
    EXPECT_EQ(sortedElements[2]->m_expandedPaths.size(), 0);
}

void libTestUtils::TestProjectData::testProjectData(const babelwires::ProjectData& projectData) {
    libTestUtils::TestRecordFeature testRecord;
    libTestUtils::TestFileFeature testFileFeature;

    libTestUtils::TestProjectData::testProjectDataAndDisciminators(
        projectData, testRecord.m_intId.getDiscriminator(), testRecord.m_arrayId.getDiscriminator(),
        testRecord.m_recordId.getDiscriminator(), testRecord.m_int2Id.getDiscriminator(),
        testFileFeature.m_intChildId.getDiscriminator());
}

void libTestUtils::TestProjectData::resolvePathsInCurrentContext() {
    libTestUtils::TestRecordFeature testRecord;
    libTestUtils::TestFileFeature testFileFeature;

    // These have side-effects on the mutable field discriminators in the paths.
    auto modData0 = m_elements[0]->m_modifiers[0].get()->as<babelwires::ConnectionModifierData>();
    modData0->m_pathToFeature.tryFollow(testFileFeature);
    modData0->m_pathToSourceFeature.tryFollow(testRecord);
    auto modData1 = m_elements[1]->m_modifiers[0].get()->as<babelwires::ConnectionModifierData>();
    modData1->m_pathToFeature.tryFollow(testRecord);
    modData1->m_pathToSourceFeature.tryFollow(testFileFeature);
    auto modData2 = m_elements[1]->m_modifiers[1].get()->as<babelwires::IntValueAssignmentData>();
    modData2->m_pathToFeature.tryFollow(testRecord);
    m_elements[1]->m_expandedPaths[0].tryFollow(testRecord);
}
