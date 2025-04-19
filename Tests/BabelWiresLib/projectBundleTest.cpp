#include <gtest/gtest.h>

#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Serialization/projectBundle.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <Domains/TestDomain/testFileFormats.hpp>
#include <Domains/TestDomain/testProcessor.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testProjectData.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>

/*

// This test needs to be rethought: It's too brittle to changes in built-in types and test data.

TEST(ProjectBundleTest, fieldIdsInPaths) {
    // This will carry data between the first part of the test and the second.
    babelwires::ProjectBundle bundle;

    {
        testUtils::TestEnvironment testEnvironment;

        // Confirm that not all the discriminators in a test record are default.
        {
            // A failure in this section _can_ indicate that one of the identifiers is now being used in BabelWires
            // itself or one of the features or types registered in the TestEnvironment.
            testUtils::TestRecordFeature testRecord;
            testUtils::TestFileFeature testFileFeature(testEnvironment.m_projectContext);
            EXPECT_EQ(babelwires::getPathTo(testRecord.m_int).getLastStep().asField()->getDiscriminator(), 4);
            EXPECT_EQ(babelwires::getPathTo(testRecord.m_array).getLastStep().asField()->getDiscriminator(),
                      3);
            EXPECT_EQ(
                babelwires::getPathTo(testRecord.m_subRecord).getLastStep().asField()->getDiscriminator(), 2);
            EXPECT_EQ(babelwires::getPathTo(testRecord.m_intFeature2).getLastStep().asField()->getDiscriminator(), 1);
            EXPECT_EQ(
                babelwires::getPathTo(testFileFeature.m_intChildFeature).getLastStep().asField()->getDiscriminator(),
                3);

            // Sanity check that the ids are unaffected by the registration re-running.
            testUtils::TestRecordFeature testRecord2;
            testUtils::TestFileFeature testFileFeature2(testEnvironment.m_projectContext);
            EXPECT_EQ(babelwires::getPathTo(testRecord2.m_int).getLastStep().asField()->getDiscriminator(), 4);
            EXPECT_EQ(babelwires::getPathTo(testRecord2.m_array).getLastStep().asField()->getDiscriminator(),
                      3);
            EXPECT_EQ(
                babelwires::getPathTo(testRecord2.m_subRecord).getLastStep().asField()->getDiscriminator(), 2);
            EXPECT_EQ(babelwires::getPathTo(testRecord2.m_intFeature2).getLastStep().asField()->getDiscriminator(),
                      1);
            EXPECT_EQ(
                babelwires::getPathTo(testFileFeature2.m_intChildFeature).getLastStep().asField()->getDiscriminator(),
                3);
        }

        testUtils::TestProjectData projectData;

        // Resolve the paths, to ensure their field discriminators map to the registered fields.
        {
            // First confirm that the paths in the project data are as expected and have not yet been resolved
            testUtils::TestProjectData::testProjectDataAndDisciminators(projectData, 0, 0, 0, 0, 0);
            projectData.resolvePathsInCurrentContext(testEnvironment.m_projectContext);
            testUtils::TestProjectData::testProjectData(testEnvironment.m_projectContext, projectData);
        }

        // Test the construction of a bundle from a projectData.
        babelwires::ProjectBundle bundle2(std::filesystem::current_path(), std::move(projectData));
        bundle2.interpretInCurrentContext();

        {
            const babelwires::IdentifierRegistry& testRegistry = bundle2.getIdentifierRegistry();

            // These have no duplicates in the bundle.
            const int recordIntDiscriminator = 1;
            const int recordArrayDiscriminator = 1;
            const int recordRecordDiscriminator = 1;

            // These duplicate each other, so we don't know how they will be discriminated.
            int recordInt2Disciminator = 0;
            int fileIntChildDiscriminator = 0;

            int entries = 0;
            int unrecognizedEntries = 0;
            for (const auto& v : testRegistry) {
                ++entries;
                const babelwires::LongId& fieldIdentifier = std::get<0>(v);
                const std::string& fieldName = *std::get<1>(v);
                const babelwires::Uuid& uuid = *std::get<2>(v);
                if (uuid == testDomain::TestProcessorInputOutputType::s_intUuid) {
                    EXPECT_EQ(fieldName, testDomain::TestProcessorInputOutputType::s_intFieldName);
                    EXPECT_EQ(fieldIdentifier, testDomain::TestProcessorInputOutputType::s_intIdInitializer);
                    EXPECT_EQ(fieldIdentifier.getDiscriminator(), recordIntDiscriminator);
                } else if (uuid == testDomain::TestProcessorInputOutputType::s_arrayUuid) {
                    EXPECT_EQ(fieldName, testDomain::TestProcessorInputOutputType::s_arrayFieldName);
                    EXPECT_EQ(fieldIdentifier, testDomain::TestProcessorInputOutputType::s_arrayIdInitializer);
                    EXPECT_EQ(fieldIdentifier.getDiscriminator(), recordArrayDiscriminator);
                } else if (uuid == testDomain::TestProcessorInputOutputType::s_recordUuid) {
                    EXPECT_EQ(fieldName, testDomain::TestProcessorInputOutputType::s_recordFieldName);
                    EXPECT_EQ(fieldIdentifier, testDomain::TestProcessorInputOutputType::s_recordIdInitializer);
                    EXPECT_EQ(fieldIdentifier.getDiscriminator(), recordRecordDiscriminator);
                } else if (uuid == testDomain::TestSimpleRecordType::s_int0Uuid) {
                    EXPECT_EQ(fieldName, testDomain::TestSimpleRecordType::s_int0FieldName);
                    EXPECT_EQ(fieldIdentifier, testDomain::TestSimpleRecordType::s_int0IdInitializer);
                    recordInt2Disciminator = fieldIdentifier.getDiscriminator();
                    EXPECT_GE(recordInt2Disciminator, 1);
                    EXPECT_LE(recordInt2Disciminator, 2);
                } else if (uuid == testUtils::TestFileFeature::s_intChildUuid) {
                    EXPECT_EQ(fieldName, testUtils::TestFileFeature::s_intChildFieldName);
                    EXPECT_EQ(fieldIdentifier, testUtils::TestFileFeature::s_intChildInitializer);
                    fileIntChildDiscriminator = fieldIdentifier.getDiscriminator();
                    EXPECT_GE(fileIntChildDiscriminator, 1);
                    EXPECT_LE(fileIntChildDiscriminator, 2);
                } else {
                    ++unrecognizedEntries;
                }
            }
            EXPECT_GT(entries, 5);
            //Test factory names may or may not be included, depending on whether the test code registers them properly.
            //EXPECT_EQ(unrecognizedEntries, 0);
            EXPECT_NE(recordInt2Disciminator, fileIntChildDiscriminator);

            testUtils::TestProjectData::testProjectDataAndDisciminators(
                bundle2.getData(), recordIntDiscriminator, recordArrayDiscriminator, recordRecordDiscriminator,
                recordInt2Disciminator, fileIntChildDiscriminator);
        }
        bundle = std::move(bundle2);
    }

    {
        testUtils::TestEnvironment testEnvironment;

        babelwires::ProjectData projectData = std::move(bundle).resolveAgainstCurrentContext(
            testEnvironment.m_projectContext, std::filesystem::current_path(), testEnvironment.m_log);

        testUtils::TestProjectData::testProjectData(testEnvironment.m_projectContext, projectData);
    }
}
*/

TEST(ProjectBundleTest, factoryMetadata) {
    testUtils::TestEnvironment testEnvironment;
    testUtils::TestProjectData projectData;

    // Older than registered.
    projectData.m_nodes[0]->m_factoryVersion = 1;
    // Same
    projectData.m_nodes[1]->m_factoryVersion = 2;
    // Newer than registered.
    projectData.m_nodes[2]->m_factoryVersion = 3;

    babelwires::ProjectBundle bundle(std::filesystem::current_path(), std::move(projectData));
    bundle.interpretInCurrentContext();

    ASSERT_EQ(bundle.getFactoryMetadata().size(), 3);
    EXPECT_EQ(bundle.getFactoryMetadata().find(testDomain::TestTargetFileFormat::getThisIdentifier())->second, 1);
    EXPECT_EQ(bundle.getFactoryMetadata().find(testDomain::TestProcessor::getFactoryIdentifier())->second, 2);
    EXPECT_EQ(bundle.getFactoryMetadata().find(testDomain::TestSourceFileFormat::getThisIdentifier())->second, 3);

    babelwires::ProjectData resolvedData = std::move(bundle).resolveAgainstCurrentContext(
        testEnvironment.m_projectContext, std::filesystem::current_path(), testEnvironment.m_log);

    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase(
        "Data for the factory \"Test Target File Format\" (TestTargetFormat) corresponds to an old version (1)"));
    EXPECT_FALSE(testEnvironment.m_log.hasSubstringIgnoreCase("Data for the factory \"testProcessor\""));
    EXPECT_TRUE(testEnvironment.m_log.hasSubstringIgnoreCase(
        "Data for the factory \"Test Source File Format\" (TestSourceFormat) has an unknown version (3)"));
}

TEST(ProjectBundleTest, filePathResolution) {
    testUtils::TestLogWithListener log;
    testUtils::TestEnvironment testEnvironment;

    std::filesystem::path root = std::filesystem::canonical(std::filesystem::temp_directory_path());

    testUtils::TempDirectory FooBar("Foo/Bar");
    testUtils::TempDirectory OomBar("Oom/Bar");

    testUtils::TempFilePath fileWhichExists("Foo/Bar/Bar.boo");
    fileWhichExists.ensureExists();

    testUtils::TempFilePath fileWhichExists2("Oom/Bar/Bar.boo");
    fileWhichExists2.ensureExists();

    struct TestScenario {
        std::filesystem::path m_pathInProjectData;
        std::filesystem::path m_newBase;
        std::filesystem::path m_oldBase;
        std::filesystem::path m_expectedResolvedPath;
    };

    // Based on the tests in FilePathTest.
    // Because we interpret / resolve on the same platform, some of those tests do not carry over.
    std::vector<TestScenario> scenarios = {
        // Same testEnvironment.
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo/Bar", root / "Foo/Bar", root / "Foo/Bar/Bar.boo"},
        // Same testEnvironment.
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo", root / "Foo", root / "Foo/Bar/Bar.boo"},
        // Same testEnvironment, file doesn't exist.
        TestScenario{root / "Flerm/Bar/Bar.boo", root / "Flerm", root / "Flerm", root / "Flerm/Bar/Bar.boo"},
        // New exists, old doesn't.
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo", root / "Flerm", root / "Foo/Bar/Bar.boo"},
        // Old exists, new doesn't.
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo", root / "Flerm", root / "Foo/Bar/Bar.boo"},
        // Both exist. New prefered. Check log.
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo", root / "Oom", root / "Foo/Bar/Bar.boo"},
        // Empty new
        TestScenario{root / "Foo/Bar/Bar.boo", std::filesystem::path(), root / "Foo", root / "Foo/Bar/Bar.boo"},
        // Empty old
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo", std::filesystem::path(), root / "Foo/Bar/Bar.boo"}};

    // Check that the files in the element data get resolved in the expected way were the project to be opened
    // in a different place.
    for (const auto& scenario : scenarios) {
        babelwires::ProjectBundle bundle;
        {
            babelwires::ProjectData projectData;
            {
                babelwires::SourceFileNodeData elementData;
                elementData.m_filePath = scenario.m_pathInProjectData;
                elementData.m_factoryIdentifier = "MyFactory";
                projectData.m_nodes.emplace_back(elementData.clone());
            }
            {
                babelwires::TargetFileNodeData elementData;
                elementData.m_filePath = scenario.m_pathInProjectData;
                elementData.m_factoryIdentifier = "MyOtherFactory";
                projectData.m_nodes.emplace_back(elementData.clone());
            }
            bundle = babelwires::ProjectBundle(scenario.m_oldBase, std::move(projectData));
        }

        babelwires::ProjectData projectData;
        projectData =
            std::move(bundle).resolveAgainstCurrentContext(testEnvironment.m_projectContext, scenario.m_newBase, log);

        ASSERT_EQ(projectData.m_nodes.size(), 2);
        {
            auto elementData = projectData.m_nodes[0].get()->as<babelwires::SourceFileNodeData>();
            ASSERT_NE(elementData, nullptr);
            EXPECT_EQ(elementData->m_filePath, scenario.m_expectedResolvedPath);
        }
        {
            auto elementData = projectData.m_nodes[1].get()->as<babelwires::TargetFileNodeData>();
            ASSERT_NE(elementData, nullptr);
            EXPECT_EQ(elementData->m_filePath, scenario.m_expectedResolvedPath);
        }
    }
}

/*
TEST(ProjectBundleTest, factoryIdentifiers) {
    testUtils::TestLog log;

    // Prepopulate the identifierRegistry with clashing factory identifier.
    // I don't expect duplicate factory identifiers, but this will make it easier to test
    babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(testUtils::TestProcessorFactory::getThisType(),
                                                             "Other test processor",
"41000000-1111-2222-3333-888888888888", babelwires::IdentifierRegistry::Authority::isAuthoritative);
    babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(testDomain::TestSourceFileFormat::getThisType(),
                                                             "Other test source factory",
"41000000-1111-2222-3333-999999999999", babelwires::IdentifierRegistry::Authority::isAuthoritative);
    babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(testDomain::TestTargetFileFormat::getThisType(),
                                                             "Other test target factory",
"41000000-1111-2222-3333-aaaaaaaaaaaa", babelwires::IdentifierRegistry::Authority::isAuthoritative);

    testUtils::TestProjectData projectData;

    EXPECT_EQ(projectData.m_elements[0]->m_factoryIdentifier.getDiscriminator(), 0);
    EXPECT_EQ(projectData.m_elements[1]->m_factoryIdentifier.getDiscriminator(), 0);
    EXPECT_EQ(projectData.m_elements[2]->m_factoryIdentifier.getDiscriminator(), 0);

    testUtils::TestEnvironment testEnvironment;
    testEnvironment.m_projectContext.m_targetFileFormatReg.getEntryByIdentifier(projectData.m_elements[0]->m_factoryIdentifier);
    testEnvironment.m_projectContext.m_processorReg.getEntryByIdentifier(projectData.m_elements[1]->m_factoryIdentifier);
    testEnvironment.m_projectContext.m_sourceFileFormatReg.getEntryByIdentifier(projectData.m_elements[2]->m_factoryIdentifier);

    EXPECT_EQ(projectData.m_elements[0]->m_factoryIdentifier.getDiscriminator(), 2);
    EXPECT_EQ(projectData.m_elements[1]->m_factoryIdentifier.getDiscriminator(), 2);
    EXPECT_EQ(projectData.m_elements[2]->m_factoryIdentifier.getDiscriminator(), 2);

    babelwires::ProjectBundle bundle(std::filesystem::current_path(), std::move(projectData));
    bundle.interpretInCurrentContext();

    EXPECT_EQ(bundle.getData().m_elements[0]->m_factoryIdentifier.getDiscriminator(), 1);
    EXPECT_EQ(bundle.getData().m_elements[1]->m_factoryIdentifier.getDiscriminator(), 1);
    EXPECT_EQ(bundle.getData().m_elements[2]->m_factoryIdentifier.getDiscriminator(), 1);
}
*/