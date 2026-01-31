#include <gtest/gtest.h>

#include <BaseLib/DataContext/dataBundle.hpp>
#include <BaseLib/DataContext/dataSerialization.hpp>
#include <BaseLib/DataContext/dataVisitable.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

#include <Tests/TestUtils/tempFilePath.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    struct TestBundlePayload : babelwires::Serializable, babelwires::DataVisitable {
        SERIALIZABLE(TestBundlePayload, "TestBundlePayload", void, 1);

        TestBundlePayload() {}
        TestBundlePayload(int x)
            : m_contents(x)
            , m_shortId(babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
                  "short", "short", "00000000-1111-2222-3333-000000000000",
                  babelwires::IdentifierRegistry::Authority::isAuthoritative))
            , m_mediumId(babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
                  "mediumId", "medium", "00000000-1111-2222-3333-000000000001",
                  babelwires::IdentifierRegistry::Authority::isAuthoritative))
            , m_longId(babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
                  "aLongIdentifier", "long", "00000000-1111-2222-3333-000000000002",
                  babelwires::IdentifierRegistry::Authority::isAuthoritative)) {}

        void visitIdentifiers(babelwires::IdentifierVisitor& visitor) override {
            visitor(m_shortId);
            visitor(m_mediumId);
            visitor(m_longId);
        }

        /// Call the visitor on all FilePaths in the object.
        void visitFilePaths(babelwires::FilePathVisitor& visitor) override { visitor(m_filePath); }

        void serializeContents(babelwires::Serializer& serializer) const override {
            serializer.serializeValue("shortId", m_shortId);
            serializer.serializeValue("mediumId", m_mediumId);
            serializer.serializeValue("longId", m_longId);
            serializer.serializeValue("filePath", m_filePath);
        }

        void deserializeContents(babelwires::Deserializer& deserializer) override {
            THROW_ON_ERROR(deserializer.deserializeValue("shortId", m_shortId), babelwires::ParseException);
            THROW_ON_ERROR(deserializer.deserializeValue("mediumId", m_mediumId), babelwires::ParseException);
            THROW_ON_ERROR(deserializer.deserializeValue("longId", m_longId), babelwires::ParseException);
            THROW_ON_ERROR(deserializer.deserializeValue("filePath", m_filePath), babelwires::ParseException);
        }

        int m_contents;
        babelwires::ShortId m_shortId;
        babelwires::MediumId m_mediumId;
        babelwires::LongId m_longId;
        babelwires::FilePath m_filePath;
    };

    struct TestBundle : babelwires::DataBundle<TestBundlePayload> {
        SERIALIZABLE(TestBundle, "TestBundle", void, 1);

        TestBundle() = default;
        TestBundle(std::filesystem::path pathToBundleFile, TestBundlePayload&& payload)
            : DataBundle(std::move(pathToBundleFile), std::move(payload)) {}

        void visitIdentifiers(babelwires::IdentifierVisitor& visitor) override { getData().visitIdentifiers(visitor); }

        /// Call the visitor on all FilePaths in the object.
        void visitFilePaths(babelwires::FilePathVisitor& visitor) override { getData().visitFilePaths(visitor); }

        void interpretAdditionalMetadataInCurrentContext() override {}

        void adaptDataToAdditionalMetadata(const babelwires::DataContext& context,
                                           babelwires::UserLogger& userLogger) override {}

        void serializeAdditionalMetadata(babelwires::Serializer& serializer) const override {}

        void deserializeAdditionalMetadata(babelwires::Deserializer& deserializer) override {}
    };
} // namespace

TEST(DataBundleTest, identifiers) {
    // This will carry data between the first part of the test and the second.
    TestBundle bundle;

    {
        testUtils::TestLog testLog;
        babelwires::IdentifierRegistryScope identifierRegistry;

        // Ensure we're not just testing in same context on both sides.
        babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
            "mediumId", "other medium", "55555555-1111-2222-3333-000000000001",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
            "aLongIdentifier", "other long 1", "55555555-1111-2222-3333-000000000002",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
            "aLongIdentifier", "other long 2", "55555555-1111-2222-3333-000000000022",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);

        TestBundlePayload sourcePayload(15);

        EXPECT_EQ(sourcePayload.m_shortId.getDiscriminator(), 1);
        EXPECT_EQ(sourcePayload.m_mediumId.getDiscriminator(), 2);
        EXPECT_EQ(sourcePayload.m_longId.getDiscriminator(), 3);

        bundle = TestBundle(std::filesystem::current_path(), std::move(sourcePayload));
        bundle.interpretInCurrentContext();

        EXPECT_EQ(bundle.getData().m_shortId.getDiscriminator(), 1);
        EXPECT_EQ(bundle.getData().m_mediumId.getDiscriminator(), 1);
        EXPECT_EQ(bundle.getData().m_longId.getDiscriminator(), 1);
    }

    {
        testUtils::TestLog testLog;
        babelwires::IdentifierRegistryScope identifierRegistry;
        babelwires::AutomaticDeserializationRegistry deserializationReg;
        babelwires::DataContext dataContext{deserializationReg};

        // Ensure we're not just testing in same context on both sides.
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            "short", "other short 1", "66666666-1111-2222-3333-000000000000",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addShortIdWithMetadata(
            "short", "other short 2", "66666666-1111-2222-3333-000000000011",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);
        babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
            "mediumId", "other medium", "66666666-1111-2222-3333-000000000001",
            babelwires::IdentifierRegistry::Authority::isAuthoritative);

        TestBundlePayload targetPayload =
            std::move(bundle).resolveAgainstCurrentContext(dataContext, std::filesystem::current_path(), testLog);

        EXPECT_EQ(targetPayload.m_contents, 15);
        EXPECT_EQ(targetPayload.m_shortId.getDiscriminator(), 3);
        EXPECT_EQ(targetPayload.m_mediumId.getDiscriminator(), 2);
        EXPECT_EQ(targetPayload.m_longId.getDiscriminator(), 1);

        // Confirm that the resolved data is provisional.
        {
            babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
                "mediumId", "Updated name", "00000000-1111-2222-3333-000000000001",
                babelwires::IdentifierRegistry::Authority::isAuthoritative);
            EXPECT_EQ(babelwires::IdentifierRegistry::read()->getName(targetPayload.m_mediumId), "Updated name");
        }
    }
}

TEST(DataBundleTest, filePathResolution) {
    testUtils::TestLog log;

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
        TestBundle bundle;
        {
            testUtils::TestLog testLog;
            babelwires::IdentifierRegistryScope identifierRegistry;

            TestBundlePayload sourcePayload(44);
            sourcePayload.m_filePath = scenario.m_pathInProjectData;
            bundle = TestBundle(scenario.m_oldBase, std::move(sourcePayload));
            bundle.interpretInCurrentContext();
        }

        {
            testUtils::TestLog testLog;
            babelwires::IdentifierRegistryScope identifierRegistry;
            babelwires::AutomaticDeserializationRegistry deserializationReg;
            babelwires::DataContext dataContext{deserializationReg};

            TestBundlePayload targetPayload =
                std::move(bundle).resolveAgainstCurrentContext(dataContext, scenario.m_newBase, log);

            EXPECT_EQ(targetPayload.m_contents, 44);
            EXPECT_EQ(targetPayload.m_filePath, scenario.m_expectedResolvedPath);
        }
    }
}
