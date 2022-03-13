#include <gtest/gtest.h>

#include "Common/Identifiers/identifierRegistry.hpp"
#include "BabelWiresLib/Serialization/projectSerialization.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectData.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"
#include "Tests/TestUtils/tempFilePath.hpp"

#include "Tests/TestUtils/testLog.hpp"

TEST(ProjectSerializationTest, saveLoadStringSameContext) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

    libTestUtils::TestProjectData testProjectData;
    testProjectData.resolvePathsInCurrentContext(context.m_projectContext);

    // TODO: FilePaths not properly handled here.
    const std::string serializedContents = babelwires::ProjectSerialization::saveToString(std::filesystem::current_path(), std::move(testProjectData));

    babelwires::ProjectData loadedData =
        babelwires::ProjectSerialization::loadFromString(serializedContents, context.m_projectContext, std::filesystem::current_path(), context.m_log);

    libTestUtils::TestRecordFeature testRecord;
    libTestUtils::TestFileFeature testFileFeature(context.m_projectContext);

    libTestUtils::TestProjectData::testProjectDataAndDisciminators(
        loadedData, testRecord.m_intId.getDiscriminator(), testRecord.m_arrayId.getDiscriminator(),
        testRecord.m_recordId.getDiscriminator(), testRecord.m_int2Id.getDiscriminator(),
        testFileFeature.m_intChildId.getDiscriminator());
}

TEST(ProjectSerializationTest, saveLoadStringSeparateContext) {
    std::string serializedContents;
    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        libTestUtils::TestProjectData testProjectData;
        testProjectData.resolvePathsInCurrentContext(context.m_projectContext);

        serializedContents = babelwires::ProjectSerialization::saveToString(std::filesystem::current_path(), std::move(testProjectData));
    }

    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        babelwires::ProjectData loadedData = babelwires::ProjectSerialization::loadFromString(
            serializedContents, context.m_projectContext, std::filesystem::current_path(), context.m_log);

        libTestUtils::TestRecordFeature testRecord;
        libTestUtils::TestFileFeature testFileFeature(context.m_projectContext);

        libTestUtils::TestProjectData::testProjectDataAndDisciminators(
            loadedData, testRecord.m_intId.getDiscriminator(), testRecord.m_arrayId.getDiscriminator(),
            testRecord.m_recordId.getDiscriminator(), testRecord.m_int2Id.getDiscriminator(),
            testFileFeature.m_intChildId.getDiscriminator());
    }
}

TEST(ProjectSerializationTest, saveLoadFile) {
    testUtils::TempFilePath tempFile("Foo.test");
    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        libTestUtils::TestProjectData testProjectData;
        testProjectData.resolvePathsInCurrentContext(context.m_projectContext);

        babelwires::ProjectSerialization::saveToFile(tempFile, std::move(testProjectData));
    }

    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext context;

        babelwires::ProjectData loadedData =
            babelwires::ProjectSerialization::loadFromFile(tempFile, context.m_projectContext, context.m_log);

        libTestUtils::TestProjectData::testProjectData(context.m_projectContext, loadedData);
    }
}
