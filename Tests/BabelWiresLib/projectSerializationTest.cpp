#include <gtest/gtest.h>

#include <BabelWiresLib/Serialization/projectSerialization.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testFileFormats.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>
#include <Tests/BabelWiresLib/TestUtils/testRecord.hpp>
#include <Tests/TestUtils/tempFilePath.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(ProjectSerializationTest, saveLoadStringSameContext) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestEnvironment testEnvironment;

    testUtils::TestProjectData testProjectData;
    testProjectData.resolvePathsInCurrentContext(testEnvironment.m_projectContext);

    // TODO: FilePaths not properly handled here.
    const std::string serializedContents = babelwires::ProjectSerialization::saveToString(std::filesystem::current_path(), std::move(testProjectData));

    babelwires::ProjectData loadedData =
        babelwires::ProjectSerialization::loadFromString(serializedContents, testEnvironment.m_projectContext, std::filesystem::current_path(), testEnvironment.m_log);

    testUtils::TestRecordFeature testRecord;
    testUtils::TestFileFeature testFileFeature(testEnvironment.m_projectContext);

    testUtils::TestProjectData::testProjectDataAndDisciminators(
        loadedData, testRecord.m_intId.getDiscriminator(), testRecord.m_arrayId.getDiscriminator(),
        testRecord.m_recordId.getDiscriminator(), testRecord.m_int2Id.getDiscriminator(),
        testFileFeature.m_intChildId.getDiscriminator());
}

TEST(ProjectSerializationTest, saveLoadStringSeparateContext) {
    std::string serializedContents;
    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        testUtils::TestEnvironment testEnvironment;

        testUtils::TestProjectData testProjectData;
        testProjectData.resolvePathsInCurrentContext(testEnvironment.m_projectContext);

        serializedContents = babelwires::ProjectSerialization::saveToString(std::filesystem::current_path(), std::move(testProjectData));
    }

    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        testUtils::TestEnvironment testEnvironment;

        babelwires::ProjectData loadedData = babelwires::ProjectSerialization::loadFromString(
            serializedContents, testEnvironment.m_projectContext, std::filesystem::current_path(), testEnvironment.m_log);

        testUtils::TestRecordFeature testRecord;
        testUtils::TestFileFeature testFileFeature(testEnvironment.m_projectContext);

        testUtils::TestProjectData::testProjectDataAndDisciminators(
            loadedData, testRecord.m_intId.getDiscriminator(), testRecord.m_arrayId.getDiscriminator(),
            testRecord.m_recordId.getDiscriminator(), testRecord.m_int2Id.getDiscriminator(),
            testFileFeature.m_intChildId.getDiscriminator());
    }
}

TEST(ProjectSerializationTest, saveLoadFile) {
    testUtils::TempFilePath tempFile("Foo.test");
    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        testUtils::TestEnvironment testEnvironment;

        testUtils::TestProjectData testProjectData;
        testProjectData.resolvePathsInCurrentContext(testEnvironment.m_projectContext);

        babelwires::ProjectSerialization::saveToFile(tempFile, std::move(testProjectData));
    }

    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        testUtils::TestEnvironment testEnvironment;

        babelwires::ProjectData loadedData =
            babelwires::ProjectSerialization::loadFromFile(tempFile, testEnvironment.m_projectContext, testEnvironment.m_log);

        testUtils::TestProjectData::testProjectData(testEnvironment.m_projectContext, loadedData);
    }
}
