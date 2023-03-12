#include <gtest/gtest.h>

#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Registry/fileTypeRegistry.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

namespace {
    struct TestRegistryEntry : public babelwires::FileTypeEntry {
      public:
        TestRegistryEntry(babelwires::LongId identifier, babelwires::VersionNumber version,
                          Extensions extensions, int payload)
            : FileTypeEntry(identifier, version, std::move(extensions))
            , m_payload(payload) {}

        int m_payload;
    };

    using TestRegistry = babelwires::FileTypeRegistry<TestRegistryEntry>;
} // namespace

// Briefly test the equalSets function.
TEST(RegistryTest, testEqualSets) {
    ASSERT_TRUE(testUtils::areEqualSets(TestRegistryEntry::Extensions{}, TestRegistryEntry::Extensions{}));
    ASSERT_FALSE(testUtils::areEqualSets(TestRegistryEntry::Extensions{"a"}, TestRegistryEntry::Extensions{}));
    ASSERT_FALSE(testUtils::areEqualSets(TestRegistryEntry::Extensions{}, TestRegistryEntry::Extensions{"b"}));
    ASSERT_TRUE(testUtils::areEqualSets(TestRegistryEntry::Extensions{"aa", "b", "ccc"},
                                        TestRegistryEntry::Extensions{"b", "aa", "ccc"}));
    ASSERT_FALSE(testUtils::areEqualSets(TestRegistryEntry::Extensions{"aa", "b", "ccc"},
                                         TestRegistryEntry::Extensions{"aa", "b", "ccc", "d"}));
}

TEST(RegistryTest, base) {
    babelwires::IdentifierRegistryScope identifierRegistry;
    testUtils::TestLog log;

    TestRegistry registry("Test registry");

    EXPECT_EQ(registry.getEntryByIdentifier("test"), nullptr);

    const babelwires::LongId entryId = babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
        "test", "Test", "00000000-1111-2222-3333-444444444444",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    registry.addEntry<TestRegistryEntry>(entryId, 1, TestRegistryEntry::Extensions{"test"}, 15);

    ASSERT_NE(registry.getEntryByIdentifier("test"), nullptr);
    EXPECT_NE(&registry.getRegisteredEntry("test"), nullptr);
    EXPECT_EQ(registry.getEntryByIdentifier("test")->getName(), "Test");
    EXPECT_TRUE(testUtils::areEqualSets(registry.getFileExtensions(), TestRegistryEntry::Extensions{"test"}));
    ASSERT_NE(registry.getEntryByFileName("foo.test"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.test")->getName(), "Test");
    EXPECT_EQ(registry.getEntryByFileName("foo.test")->m_payload, 15);
    ASSERT_NE(registry.getEntryByFileName("test"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("test")->getName(), "Test");
    EXPECT_EQ(registry.getEntryByFileName("test")->m_payload, 15);
    EXPECT_EQ(registry.getEntryByFileName("test2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("test.test2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("oom.test.test2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.test2"), nullptr);

    const babelwires::LongId entryId2 = babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
        "test2", "Test2", "00000000-1111-2222-3333-555555555555",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    registry.addEntry(
        std::make_unique<TestRegistryEntry>(entryId2, 1, TestRegistryEntry::Extensions{"test2", "TEST_2"}, -144));

    ASSERT_NE(registry.getEntryByIdentifier("test2"), nullptr);
    EXPECT_NE(&registry.getRegisteredEntry("test2"), nullptr);
    EXPECT_EQ(registry.getEntryByIdentifier("test2")->getName(), "Test2");
    EXPECT_TRUE(testUtils::areEqualSets(registry.getFileExtensions(),
                                        TestRegistryEntry::Extensions{"test", "test2", "test_2"}));
    ASSERT_NE(registry.getEntryByFileName("foo.test2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.test2")->getName(), "Test2");
    EXPECT_EQ(registry.getEntryByFileName("foo.test2")->m_payload, -144);
    ASSERT_NE(registry.getEntryByFileName("foo.TEST_2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.TEST_2")->getName(), "Test2");
    EXPECT_EQ(registry.getEntryByFileName("foo.TEST_2")->m_payload, -144);
    EXPECT_NE(registry.getEntryByFileName("oom.test.test2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("oom.test.test2")->getName(), "Test2");
    EXPECT_EQ(registry.getEntryByFileName("oom.test.test2")->m_payload, -144);

    const babelwires::LongId entryId3 = babelwires::IdentifierRegistry::write()->addLongIdWithMetadata(
        "test3", "Test3", "00000000-1111-2222-3333-666666666666",
        babelwires::IdentifierRegistry::Authority::isAuthoritative);

    registry.addEntry(
        std::make_unique<TestRegistryEntry>(entryId3, 1, TestRegistryEntry::Extensions{"foo", "bar"}, 23));

    EXPECT_TRUE(testUtils::areEqualSets(registry.getFileExtensions(),
                                        TestRegistryEntry::Extensions{"test", "test2", "test_2", "foo", "bar"}));

    ASSERT_NE(registry.getEntryByFileName("foo.TEST"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.TEST")->getName(), "Test");
    ASSERT_NE(registry.getEntryByFileName("foo.test_2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.test_2")->getName(), "Test2");
    ASSERT_NE(registry.getEntryByFileName("foo.fOo"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.fOo")->getName(), "Test3");
}
