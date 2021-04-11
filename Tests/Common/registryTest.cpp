#include <gtest/gtest.h>

#include "Common/Registry/fileTypeRegistry.hpp"
#include "Tests/TestUtils/equalSets.hpp"

namespace {
    struct TestRegistryEntry : public babelwires::FileTypeEntry {
      public:
        TestRegistryEntry(std::string identifier, std::string name, babelwires::VersionNumber version,
                          Extensions extensions, int payload)
            : FileTypeEntry(std::move(identifier), std::move(name), version, std::move(extensions))
            , m_payload(payload) {}

        int m_payload;
    };

    using TestRegistry = babelwires::FileTypeRegistry<TestRegistryEntry>;
} // namespace

TEST(RegistryTest, base) {
    // Briefly test equalSets.
    {
        ASSERT_TRUE(testUtils::areEqualSets(TestRegistryEntry::Extensions{}, TestRegistryEntry::Extensions{}));
        ASSERT_FALSE(testUtils::areEqualSets(TestRegistryEntry::Extensions{"a"}, TestRegistryEntry::Extensions{}));
        ASSERT_FALSE(testUtils::areEqualSets(TestRegistryEntry::Extensions{}, TestRegistryEntry::Extensions{"b"}));
        ASSERT_TRUE(testUtils::areEqualSets(TestRegistryEntry::Extensions{"aa", "b", "ccc"},
                                            TestRegistryEntry::Extensions{"b", "aa", "ccc"}));
        ASSERT_FALSE(testUtils::areEqualSets(TestRegistryEntry::Extensions{"aa", "b", "ccc"},
                                             TestRegistryEntry::Extensions{"aa", "b", "ccc", "d"}));
    }

    TestRegistry registry("Test registry");

    EXPECT_EQ(registry.getEntryByIdentifier("test"), nullptr);

    registry.addEntry(
        std::make_unique<TestRegistryEntry>("test", "Test", 1, TestRegistryEntry::Extensions{"test"}, 15));

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

    registry.addEntry(std::make_unique<TestRegistryEntry>("test2", "Test2", 1,
                                                          TestRegistryEntry::Extensions{"test2", "TEST_2"}, -144));

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

    registry.addEntry(
        std::make_unique<TestRegistryEntry>("test3", "Test3", 1, TestRegistryEntry::Extensions{"foo", "bar"}, 23));

    EXPECT_TRUE(testUtils::areEqualSets(registry.getFileExtensions(),
                                        TestRegistryEntry::Extensions{"test", "test2", "test_2", "foo", "bar"}));

    ASSERT_NE(registry.getEntryByFileName("foo.TEST"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.TEST")->getName(), "Test");
    ASSERT_NE(registry.getEntryByFileName("foo.test_2"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.test_2")->getName(), "Test2");
    ASSERT_NE(registry.getEntryByFileName("foo.fOo"), nullptr);
    EXPECT_EQ(registry.getEntryByFileName("foo.fOo")->getName(), "Test3");
}
