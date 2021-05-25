#include <gtest/gtest.h>

#include "BabelWires/Project/FeatureElements/sourceFileElementData.hpp"
#include "BabelWires/Project/FeatureElements/featureElementData.hpp"
#include "BabelWires/Project/projectData.hpp"

#include "Common/Serialization/XML/xmlDeserializer.hpp"
#include "Common/Serialization/XML/xmlSerializer.hpp"

#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWires/TestUtils/testProcessor.hpp"
#include "Tests/BabelWires/TestUtils/testProjectData.hpp"

#include "Tests/TestUtils/testLog.hpp"

TEST(ProjectDataTest, serialization) {
    std::string serializedContents;
    {
        libTestUtils::TestProjectData projectData;

        babelwires::XmlSerializer serializer;
        serializer.serializeObject(projectData);
        std::ostringstream os;
        serializer.write(os);
        serializedContents = std::move(os.str());
    }

    testUtils::TestLog log;
    babelwires::AutomaticDeserializationRegistry deserializationReg;
    babelwires::XmlDeserializer deserializer(serializedContents, deserializationReg, log);
    auto dataPtr = deserializer.deserializeObject<babelwires::ProjectData>();
    deserializer.finalize();

    EXPECT_EQ(dataPtr->m_projectId, 1243);
    ASSERT_EQ(dataPtr->m_elements.size(), 3);
    {
        const babelwires::ElementData* data = dataPtr->m_elements[0].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 45);
        EXPECT_EQ(data->m_factoryIdentifier, libTestUtils::TestTargetFileFormat::getThisIdentifier());
        const babelwires::TargetFileData* targetData = dynamic_cast<const babelwires::TargetFileData*>(data);
        ASSERT_TRUE(targetData);
        ASSERT_EQ(targetData->m_absoluteFilePath, libTestUtils::TestProjectData().m_targetFilePath);
    }
    {
        const babelwires::ElementData* data = dataPtr->m_elements[1].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 6);
        EXPECT_EQ(data->m_factoryIdentifier, libTestUtils::TestProcessorFactory::getThisIdentifier());
        const babelwires::ProcessorData* processorData = dynamic_cast<const babelwires::ProcessorData*>(data);
        ASSERT_TRUE(processorData);
    }
    {
        const babelwires::ElementData* data = dataPtr->m_elements[2].get();
        ASSERT_TRUE(data);
        EXPECT_EQ(data->m_id, 12);
        EXPECT_EQ(data->m_factoryIdentifier, libTestUtils::TestSourceFileFormat::getThisIdentifier());
        const babelwires::SourceFileData* sourceData = dynamic_cast<const babelwires::SourceFileData*>(data);
        ASSERT_TRUE(sourceData);
        ASSERT_EQ(sourceData->m_absoluteFilePath, libTestUtils::TestProjectData().m_sourceFilePath);
    }
}
