#include <gtest/gtest.h>

#include "BabelWires/Features/Path/fieldNameRegistry.hpp"
#include "BabelWires/Serialization/projectSerialization.hpp"

#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testProjectData.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"
#include "Tests/TestUtils/tempFilePath.hpp"

#include "Tests/TestUtils/testLog.hpp"

TEST(ProjectSerializationTest, saveLoadStringSameContext) {
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestProjectData testProjectData;
    testProjectData.resolvePathsInCurrentContext();

    const std::string serializedContents = babelwires::ProjectSerialization::saveToString(std::move(testProjectData));

    babelwires::ProjectData loadedData =
        babelwires::ProjectSerialization::loadFromString(serializedContents, context.m_projectContext, context.m_log);

    libTestUtils::TestRecordFeature testRecord;
    libTestUtils::TestFileFeature testFileFeature;

    libTestUtils::TestProjectData::testProjectDataAndDisciminators(
        loadedData, testRecord.m_intId.getDiscriminator(), testRecord.m_arrayId.getDiscriminator(),
        testRecord.m_recordId.getDiscriminator(), testRecord.m_int2Id.getDiscriminator(),
        testFileFeature.m_intChildId.getDiscriminator());
}

TEST(ProjectSerializationTest, saveLoadStringSeparateContext) {
    std::string serializedContents;
    {
        babelwires::FieldNameRegistryScope fieldNameRegistryScope;
        libTestUtils::TestProjectContext context;

        libTestUtils::TestProjectData testProjectData;
        testProjectData.resolvePathsInCurrentContext();

        serializedContents = babelwires::ProjectSerialization::saveToString(std::move(testProjectData));
    }

    {
        babelwires::FieldNameRegistryScope fieldNameRegistryScope;
        libTestUtils::TestProjectContext context;

        babelwires::ProjectData loadedData = babelwires::ProjectSerialization::loadFromString(
            serializedContents, context.m_projectContext, context.m_log);

        libTestUtils::TestRecordFeature testRecord;
        libTestUtils::TestFileFeature testFileFeature;

        libTestUtils::TestProjectData::testProjectDataAndDisciminators(
            loadedData, testRecord.m_intId.getDiscriminator(), testRecord.m_arrayId.getDiscriminator(),
            testRecord.m_recordId.getDiscriminator(), testRecord.m_int2Id.getDiscriminator(),
            testFileFeature.m_intChildId.getDiscriminator());
    }
}

TEST(ProjectSerializationTest, saveLoadFile) {
    testUtils::TempFilePath tempFile("Foo.test");
    {
        babelwires::FieldNameRegistryScope fieldNameRegistryScope;
        libTestUtils::TestProjectContext context;

        libTestUtils::TestProjectData testProjectData;
        testProjectData.resolvePathsInCurrentContext();

        babelwires::ProjectSerialization::saveToFile(tempFile, std::move(testProjectData));
    }

    {
        babelwires::FieldNameRegistryScope fieldNameRegistryScope;
        libTestUtils::TestProjectContext context;

        babelwires::ProjectData loadedData =
            babelwires::ProjectSerialization::loadFromFile(tempFile, context.m_projectContext, context.m_log);

        libTestUtils::TestProjectData::testProjectData(loadedData);
    }
}
