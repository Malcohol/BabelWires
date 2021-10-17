#include <gtest/gtest.h>

#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"
#include "BabelWiresLib/Serialization/projectBundle.hpp"
#include "BabelWiresLib/Project/FeatureElements/sourceFileElementData.hpp"
#include "BabelWiresLib/Project/FeatureElements/targetFileElementData.hpp"

#include "Tests/BabelWiresLib/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProcessor.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWiresLib/TestUtils/testProjectData.hpp"
#include "Tests/BabelWiresLib/TestUtils/testRecord.hpp"
#include "Tests/TestUtils/testLog.hpp"
#include "Tests/TestUtils/tempFilePath.hpp"

TEST(ProjectBundleTest, fieldIdsInPaths) {
    // This will carry data between the first part of the test and the second.
    babelwires::ProjectBundle bundle;

    {
        testUtils::TestLog log;
        babelwires::IdentifierRegistryScope identifierRegistry;

        // Ensure some of the test record's discriminators are not default.
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int", "41000000-1111-2222-3333-800000000001",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int 1", "42000000-1111-2222-3333-800000000001",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int 2", "43000000-1111-2222-3333-800000000001",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_arrayIdInitializer,
                                                             "test array", "41000000-1111-2222-3333-800000000002",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_arrayIdInitializer,
                                                             "test array 1", "42000000-1111-2222-3333-800000000002",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record", "41000000-1111-2222-3333-800000000003",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);

        // Also register some irrelevant field names.
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata("Flum", "Flum", "41000000-1111-2222-3333-800000000100",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata("Zarg", "Zarg", "41000000-1111-2222-3333-800000000101",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);

        // Confirm that not all the discriminators in a test record are default.
        {
            libTestUtils::TestRecordFeature testRecord;
            libTestUtils::TestFileFeature testFileFeature;
            EXPECT_EQ(babelwires::FeaturePath(testRecord.m_intFeature).getLastStep().asField()->getDiscriminator(), 4);
            EXPECT_EQ(babelwires::FeaturePath(testRecord.m_arrayFeature).getLastStep().asField()->getDiscriminator(),
                      3);
            EXPECT_EQ(
                babelwires::FeaturePath(testRecord.m_subRecordFeature).getLastStep().asField()->getDiscriminator(), 2);
            EXPECT_EQ(babelwires::FeaturePath(testRecord.m_intFeature2).getLastStep().asField()->getDiscriminator(), 1);
            EXPECT_EQ(
                babelwires::FeaturePath(testFileFeature.m_intChildFeature).getLastStep().asField()->getDiscriminator(),
                2);

            // Sanity check that the ids are unaffected by the registration re-running.
            libTestUtils::TestRecordFeature testRecord2;
            libTestUtils::TestFileFeature testFileFeature2;
            EXPECT_EQ(babelwires::FeaturePath(testRecord2.m_intFeature).getLastStep().asField()->getDiscriminator(), 4);
            EXPECT_EQ(babelwires::FeaturePath(testRecord2.m_arrayFeature).getLastStep().asField()->getDiscriminator(),
                      3);
            EXPECT_EQ(
                babelwires::FeaturePath(testRecord2.m_subRecordFeature).getLastStep().asField()->getDiscriminator(), 2);
            EXPECT_EQ(babelwires::FeaturePath(testRecord2.m_intFeature2).getLastStep().asField()->getDiscriminator(),
                      1);
            EXPECT_EQ(
                babelwires::FeaturePath(testFileFeature2.m_intChildFeature).getLastStep().asField()->getDiscriminator(),
                2);
        }

        libTestUtils::TestProjectData projectData;

        // Resolve the paths, to ensure their field discriminators map to the registered fields.
        {
            // First confirm that the paths in the project data are as expected and have not yet been resolved
            libTestUtils::TestProjectData::testProjectDataAndDisciminators(projectData, 0, 0, 0, 0, 0);
            projectData.resolvePathsInCurrentContext();
            libTestUtils::TestProjectData::testProjectDataAndDisciminators(projectData, 4, 3, 2, 1, 2);
        }

        // Test the construction of a bundle from a projectData.
        babelwires::ProjectBundle bundle2(std::filesystem::current_path(), std::move(projectData));

        {
            // Bit of a hack, but this lets us iterate through the registry.
            // TODO Perhaps just make iteration public
            const babelwires::IdentifierRegistry& testRegistry = bundle2.getFieldNameRegistry();

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
                const babelwires::Identifier& fieldIdentifier = std::get<0>(v);
                const std::string& fieldName = *std::get<1>(v);
                const babelwires::Uuid& uuid = *std::get<2>(v);
                if (uuid == libTestUtils::TestRecordFeature::s_intUuid) {
                    EXPECT_EQ(fieldName, libTestUtils::TestRecordFeature::s_intFieldName);
                    EXPECT_EQ(fieldIdentifier, libTestUtils::TestRecordFeature::s_intIdInitializer);
                    EXPECT_EQ(fieldIdentifier.getDiscriminator(), recordIntDiscriminator);
                } else if (uuid == libTestUtils::TestRecordFeature::s_arrayUuid) {
                    EXPECT_EQ(fieldName, libTestUtils::TestRecordFeature::s_arrayFieldName);
                    EXPECT_EQ(fieldIdentifier, libTestUtils::TestRecordFeature::s_arrayIdInitializer);
                    EXPECT_EQ(fieldIdentifier.getDiscriminator(), recordArrayDiscriminator);
                } else if (uuid == libTestUtils::TestRecordFeature::s_recordUuid) {
                    EXPECT_EQ(fieldName, libTestUtils::TestRecordFeature::s_recordFieldName);
                    EXPECT_EQ(fieldIdentifier, libTestUtils::TestRecordFeature::s_recordIdInitializer);
                    EXPECT_EQ(fieldIdentifier.getDiscriminator(), recordRecordDiscriminator);
                } else if (uuid == libTestUtils::TestRecordFeature::s_int2Uuid) {
                    EXPECT_EQ(fieldName, libTestUtils::TestRecordFeature::s_int2FieldName);
                    EXPECT_EQ(fieldIdentifier, libTestUtils::TestRecordFeature::s_int2IdInitializer);
                    recordInt2Disciminator = fieldIdentifier.getDiscriminator();
                    EXPECT_GE(recordInt2Disciminator, 1);
                    EXPECT_LE(recordInt2Disciminator, 2);
                } else if (uuid == libTestUtils::TestFileFeature::s_intChildUuid) {
                    EXPECT_EQ(fieldName, libTestUtils::TestFileFeature::s_intChildFieldName);
                    EXPECT_EQ(fieldIdentifier, libTestUtils::TestFileFeature::s_intChildInitializer);
                    fileIntChildDiscriminator = fieldIdentifier.getDiscriminator();
                    EXPECT_GE(fileIntChildDiscriminator, 1);
                    EXPECT_LE(fileIntChildDiscriminator, 2);
                } else {
                    ++unrecognizedEntries;
                }
            }
            EXPECT_EQ(entries, 5);
            EXPECT_EQ(unrecognizedEntries, 0);
            EXPECT_NE(recordInt2Disciminator, fileIntChildDiscriminator);

            libTestUtils::TestProjectData::testProjectDataAndDisciminators(
                bundle2.getProjectData(), recordIntDiscriminator, recordArrayDiscriminator, recordRecordDiscriminator,
                recordInt2Disciminator, fileIntChildDiscriminator);
        }
        bundle = std::move(bundle2);
    }

    {
        babelwires::IdentifierRegistryScope identifierRegistry;
        libTestUtils::TestProjectContext projectContext;

        // Slightly different arrangement and UUIDs to the above (not that it should matter)
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int", "51000000-1111-2222-3333-800000000001",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_arrayIdInitializer,
                                                             "test array", "51000000-1111-2222-3333-800000000002",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record 1", "51000000-1111-2222-3333-800000000003",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record 2", "52000000-1111-2222-3333-800000000003",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record 3", "53000000-1111-2222-3333-800000000003",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);

        // Ensure the record's fieldIdentifiers are registered, but don't do the same for the file feature.
        libTestUtils::TestRecordFeature testRecord;

        // Also register some irrelevant field names.
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata("Flum", "Flum", "51000000-1111-2222-3333-800000000100",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata("Zarg", "Zarg", "51000000-1111-2222-3333-800000000101",
                                                             babelwires::IdentifierRegistry::Authority::isAuthoritative);

        babelwires::ProjectData projectData =
            std::move(bundle).resolveAgainstCurrentContext(projectContext.m_projectContext, std::filesystem::current_path(), projectContext.m_log);

        libTestUtils::TestProjectData::testProjectDataAndDisciminators(projectData, 2, 2, 4, 1, 2);

        // Confirm that the resolved data is provisional.
        {
            babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(
                libTestUtils::TestFileFeature::s_intChildInitializer, "Updated field name",
                libTestUtils::TestFileFeature::s_intChildUuid,
                babelwires::IdentifierRegistry::Authority::isAuthoritative);
            EXPECT_EQ(babelwires::IdentifierRegistry::read()->getName(
                          *projectData.m_elements[0]->m_modifiers[0]->m_pathToFeature.getStep(0).asField()),
                      "Updated field name");
        }
    }
}

TEST(ProjectBundleTest, factoryMetadata) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestProjectData projectData;

    // Older than registered.
    projectData.m_elements[0]->m_factoryVersion = 1;
    // Same
    projectData.m_elements[1]->m_factoryVersion = 2;
    // Newer than registered.
    projectData.m_elements[2]->m_factoryVersion = 3;

    babelwires::ProjectBundle bundle(std::filesystem::current_path(), std::move(projectData));

    ASSERT_EQ(bundle.getFactoryMetadata().size(), 3);
    EXPECT_EQ(bundle.getFactoryMetadata().find(libTestUtils::TestTargetFileFormat::getThisIdentifier())->second, 1);
    EXPECT_EQ(bundle.getFactoryMetadata().find(libTestUtils::TestProcessorFactory::getThisIdentifier())->second, 2);
    EXPECT_EQ(bundle.getFactoryMetadata().find(libTestUtils::TestSourceFileFormat::getThisIdentifier())->second, 3);

    babelwires::ProjectData resolvedData =
        std::move(bundle).resolveAgainstCurrentContext(context.m_projectContext, std::filesystem::current_path(), context.m_log);

    EXPECT_TRUE(context.m_log.hasSubstringIgnoreCase(
        "Data for the factory \"testFactoryFormat\" (testFactoryFormat) corresponds to an old version (1)"));
    EXPECT_FALSE(context.m_log.hasSubstringIgnoreCase("Data for the factory \"testProcessor\""));
    EXPECT_TRUE(context.m_log.hasSubstringIgnoreCase(
        "Data for the factory \"testFileFormat\" (testFileFormat) has an unknown version (3)"));
}

TEST(ProjectBundleTest, filePathResolution) {
    testUtils::TestLogWithListener log;
    babelwires::IdentifierRegistryScope identifierRegistry;
    libTestUtils::TestProjectContext context;

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
        // Same context.
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo/Bar", root / "Foo/Bar", root / "Foo/Bar/Bar.boo"},
        // Same context.
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo", root / "Foo", root / "Foo/Bar/Bar.boo"},
        // Same context, file doesn't exist.
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
        TestScenario{root / "Foo/Bar/Bar.boo", root / "Foo", std::filesystem::path(), root / "Foo/Bar/Bar.boo"}
    };

    // Check that the files in the element data get resolved in the expected way were the project to be opened
    // in a different place.
    for (const auto& scenario : scenarios) {
        babelwires::ProjectBundle bundle;
        {
            babelwires::ProjectData projectData;
            {
                babelwires::SourceFileElementData elementData;
                elementData.m_filePath = scenario.m_pathInProjectData;
                elementData.m_factoryIdentifier = "MyFactory";
                projectData.m_elements.emplace_back(elementData.clone());
            }
            {
                babelwires::TargetFileElementData elementData;
                elementData.m_filePath = scenario.m_pathInProjectData;
                elementData.m_factoryIdentifier = "MyOtherFactory";
                projectData.m_elements.emplace_back(elementData.clone());
            }
            bundle = babelwires::ProjectBundle(scenario.m_oldBase, std::move(projectData));
        }

        babelwires::ProjectData projectData;
        projectData = std::move(bundle).resolveAgainstCurrentContext(context.m_projectContext, scenario.m_newBase, log);
        
        ASSERT_EQ(projectData.m_elements.size(), 2);
        {
            auto elementData = projectData.m_elements[0].get()->as<babelwires::SourceFileElementData>();
            ASSERT_NE(elementData, nullptr);
            EXPECT_EQ(elementData->m_filePath, scenario.m_expectedResolvedPath);
        }
        {
            auto elementData = projectData.m_elements[1].get()->as<babelwires::TargetFileElementData>();
            ASSERT_NE(elementData, nullptr);
            EXPECT_EQ(elementData->m_filePath, scenario.m_expectedResolvedPath);
        }
    }
}
