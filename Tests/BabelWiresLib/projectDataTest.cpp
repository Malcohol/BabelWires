#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileElement/targetFileElementData.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/projectData.hpp>

#include <Common/Serialization/XML/xmlDeserializer.hpp>
#include <Common/Serialization/XML/xmlSerializer.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProcessor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(ProjectDataTest, serialization) {
    testUtils::TestLog log;
    std::string serializedContents;
    {
        testUtils::TestProjectData projectData;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(projectData);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ProjectData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_projectId, 1243);
    ASSERT_EQ(dataPtr->m_elements.size(), 3);
    {
        const babelwires::NodeData* data = dataPtr->m_elements[0].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 45);
        EXPECT_EQ(data->m_factoryIdentifier, testUtils::TestTargetFileFormat::getThisIdentifier());
        const babelwires::TargetFileElementData* targetData = data->as<babelwires::TargetFileElementData>();
        ASSERT_TRUE(targetData);
        ASSERT_EQ(targetData->m_filePath, testUtils::TestProjectData().m_targetFilePath);
    }
    {
        const babelwires::NodeData* data = dataPtr->m_elements[1].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 6);
        EXPECT_EQ(data->m_factoryIdentifier, testUtils::TestProcessor::getFactoryIdentifier());
        const babelwires::ProcessorElementData* processorData = data->as<babelwires::ProcessorElementData>();
        ASSERT_TRUE(processorData);
    }
    {
        const babelwires::NodeData* data = dataPtr->m_elements[2].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 12);
        EXPECT_EQ(data->m_factoryIdentifier, testUtils::TestSourceFileFormat::getThisIdentifier());
        const babelwires::SourceFileNodeData* sourceData = data->as<babelwires::SourceFileNodeData>();
        ASSERT_TRUE(sourceData);
        ASSERT_EQ(sourceData->m_filePath, babelwires::FilePath(testUtils::TestProjectData().m_sourceFilePath));
    }
}
