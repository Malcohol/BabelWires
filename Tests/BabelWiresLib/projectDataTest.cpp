#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/projectData.hpp>

#include <BaseLib/Serialization/XML/xmlDeserializer.hpp>
#include <BaseLib/Serialization/XML/xmlSerializer.hpp>

#include <Domains/TestDomain/testFileFormats.hpp>
#include <Domains/TestDomain/testProcessor.hpp>

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
    auto dataPtrResult = deserializer.deserializeObject<babelwires::ProjectData>();
    ASSERT_TRUE(dataPtrResult);
    auto dataPtr = std::move(*dataPtrResult);
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_projectId, 1243);
    ASSERT_EQ(dataPtr->m_nodes.size(), 3);
    {
        const babelwires::NodeData* data = dataPtr->m_nodes[0].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 45);
        EXPECT_EQ(data->m_factoryIdentifier, testDomain::TestTargetFileFormat::getThisIdentifier());
        const babelwires::TargetFileNodeData* targetData = data->tryAs<babelwires::TargetFileNodeData>();
        ASSERT_TRUE(targetData);
        ASSERT_EQ(targetData->m_filePath, testUtils::TestProjectData().m_targetFilePath);
    }
    {
        const babelwires::NodeData* data = dataPtr->m_nodes[1].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 6);
        EXPECT_EQ(data->m_factoryIdentifier, testDomain::TestProcessor::getFactoryIdentifier());
        const babelwires::ProcessorNodeData* processorData = data->tryAs<babelwires::ProcessorNodeData>();
        ASSERT_TRUE(processorData);
    }
    {
        const babelwires::NodeData* data = dataPtr->m_nodes[2].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 12);
        EXPECT_EQ(data->m_factoryIdentifier, testDomain::TestSourceFileFormat::getThisIdentifier());
        const babelwires::SourceFileNodeData* sourceData = data->tryAs<babelwires::SourceFileNodeData>();
        ASSERT_TRUE(sourceData);
        ASSERT_EQ(sourceData->m_filePath, babelwires::FilePath(testUtils::TestProjectData().m_sourceFilePath));
    }
}
