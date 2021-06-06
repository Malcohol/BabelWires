#include <gtest/gtest.h>

#include "BabelWires/Project/Modifiers/modifierData.hpp"
#include "BabelWires/Serialization/projectBundle.hpp"

#include "Tests/BabelWires/TestUtils/testFileFormats.hpp"
#include "Tests/BabelWires/TestUtils/testProcessor.hpp"
#include "Tests/BabelWires/TestUtils/testProjectContext.hpp"
#include "Tests/BabelWires/TestUtils/testProjectData.hpp"
#include "Tests/BabelWires/TestUtils/testRecord.hpp"
#include "Tests/TestUtils/testLog.hpp"

namespace {
    // Hack to get at the innards of a fieldNameRegistry.
    struct TestFieldNameRegistry : babelwires::FieldNameRegistry {
        TestFieldNameRegistry(babelwires::FieldNameRegistry&& other)
            : FieldNameRegistry(std::move(other)) {}

        using babelwires::FieldNameRegistry::begin;
        using babelwires::FieldNameRegistry::const_iterator;
        using babelwires::FieldNameRegistry::end;
    };
} // namespace

TEST(ProjectBundleTest, fieldIdsInPaths) {
    // This will carry data between the first part of the test and the second.
    babelwires::ProjectBundle bundle;

    {
        testUtils::TestLog log;
        babelwires::FieldNameRegistryScope fieldNameRegistryScope;

        // Ensure some of the test record's discriminators are not default.
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int", "41000000-1111-2222-3333-800000000001",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int 1", "42000000-1111-2222-3333-800000000001",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int 2", "43000000-1111-2222-3333-800000000001",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_arrayIdInitializer,
                                                             "test array", "41000000-1111-2222-3333-800000000002",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_arrayIdInitializer,
                                                             "test array 1", "42000000-1111-2222-3333-800000000002",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record", "41000000-1111-2222-3333-800000000003",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);

        // Also register some irrelevant field names.
        babelwires::FieldNameRegistry::write()->addFieldName("Flum", "Flum", "41000000-1111-2222-3333-800000000100",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName("Zarg", "Zarg", "41000000-1111-2222-3333-800000000101",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);

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
            TestFieldNameRegistry testRegistry(std::move(bundle2.m_fieldNameRegistry));

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
                const babelwires::FieldIdentifier& fieldIdentifier = std::get<0>(v);
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
                bundle2.m_projectData, recordIntDiscriminator, recordArrayDiscriminator, recordRecordDiscriminator,
                recordInt2Disciminator, fileIntChildDiscriminator);

            // Restore the bundle's field name registry.
            bundle2.m_fieldNameRegistry = std::move(testRegistry);
        }
        bundle = std::move(bundle2);
    }

    {
        babelwires::FieldNameRegistryScope fieldNameRegistryScope;
        libTestUtils::TestProjectContext projectContext;

        // Slightly different arrangement and UUIDs to the above (not that it should matter)
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_intIdInitializer,
                                                             "test int", "51000000-1111-2222-3333-800000000001",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_arrayIdInitializer,
                                                             "test array", "51000000-1111-2222-3333-800000000002",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record 1", "51000000-1111-2222-3333-800000000003",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record 2", "52000000-1111-2222-3333-800000000003",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName(libTestUtils::TestRecordFeature::s_recordIdInitializer,
                                                             "test record 3", "53000000-1111-2222-3333-800000000003",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);

        // Ensure the record's fieldIdentifiers are registered, but don't do the same for the file feature.
        libTestUtils::TestRecordFeature testRecord;

        // Also register some irrelevant field names.
        babelwires::FieldNameRegistry::write()->addFieldName("Flum", "Flum", "51000000-1111-2222-3333-800000000100",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);
        babelwires::FieldNameRegistry::write()->addFieldName("Zarg", "Zarg", "51000000-1111-2222-3333-800000000101",
                                                             babelwires::FieldNameRegistry::Authority::isAuthoritative);

        babelwires::ProjectData projectData =
            std::move(bundle).resolveAgainstCurrentContext(projectContext.m_projectContext, std::filesystem::current_path(), projectContext.m_log);

        libTestUtils::TestProjectData::testProjectDataAndDisciminators(projectData, 2, 2, 4, 1, 2);

        // Confirm that the resolved data is provisional.
        {
            babelwires::FieldNameRegistry::write()->addFieldName(
                libTestUtils::TestFileFeature::s_intChildInitializer, "Updated field name",
                libTestUtils::TestFileFeature::s_intChildUuid,
                babelwires::FieldNameRegistry::Authority::isAuthoritative);
            EXPECT_EQ(babelwires::FieldNameRegistry::read()->getFieldName(
                          *projectData.m_elements[0]->m_modifiers[0]->m_pathToFeature.getStep(0).asField()),
                      "Updated field name");
        }
    }
}

TEST(ProjectBundleTest, factoryMetadata) {
    babelwires::FieldNameRegistryScope fieldNameRegistryScope;
    libTestUtils::TestProjectContext context;
    libTestUtils::TestProjectData projectData;

    // Older than registered.
    projectData.m_elements[0]->m_factoryVersion = 1;
    // Same
    projectData.m_elements[1]->m_factoryVersion = 2;
    // Newer than registered.
    projectData.m_elements[2]->m_factoryVersion = 3;

    babelwires::ProjectBundle bundle(std::filesystem::current_path(), std::move(projectData));

    ASSERT_EQ(bundle.m_metadata.m_factoryMetadata.size(), 3);
    EXPECT_EQ(bundle.m_metadata.m_factoryMetadata[libTestUtils::TestTargetFileFormat::getThisIdentifier()], 1);
    EXPECT_EQ(bundle.m_metadata.m_factoryMetadata[libTestUtils::TestProcessorFactory::getThisIdentifier()], 2);
    EXPECT_EQ(bundle.m_metadata.m_factoryMetadata[libTestUtils::TestSourceFileFormat::getThisIdentifier()], 3);

    babelwires::ProjectData resolvedData =
        std::move(bundle).resolveAgainstCurrentContext(context.m_projectContext, std::filesystem::current_path(), context.m_log);

    EXPECT_TRUE(context.m_log.hasSubstringIgnoreCase(
        "Data for the factory \"testFactoryFormat\" (testFactoryFormat) corresponds to an old version (1)"));
    EXPECT_FALSE(context.m_log.hasSubstringIgnoreCase("Data for the factory \"testProcessor\""));
    EXPECT_TRUE(context.m_log.hasSubstringIgnoreCase(
        "Data for the factory \"testFileFormat\" (testFileFormat) has an unknown version (3)"));
}
