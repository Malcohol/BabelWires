#include <gtest/gtest.h>

#include <BaseLib/Version/version.hpp>

TEST(VersionTest, projectVersionNotPlaceholder) {
    const babelwires::Version& v = babelwires::Version::getProjectVersion();
    EXPECT_FALSE((v.major == 0) && (v.minor == 0) && (v.patch == 0));
}

TEST(VersionTest, satisfiesStableSemver) {
    const babelwires::Version provided{1, 4, 3};

    EXPECT_TRUE(provided.satisfies({1, 4, 3}));
    EXPECT_TRUE(provided.satisfies({1, 4, 2}));
    EXPECT_TRUE(provided.satisfies({1, 3, 99}));

    EXPECT_FALSE(provided.satisfies({1, 4, 4}));
    EXPECT_FALSE(provided.satisfies({1, 5, 0}));
    EXPECT_FALSE(provided.satisfies({2, 0, 0}));
}

TEST(VersionTest, satisfiesPreOneRequiresExactMatch) {
    const babelwires::Version provided{0, 2, 3};

    EXPECT_TRUE(provided.satisfies({0, 2, 3}));
    EXPECT_FALSE(provided.satisfies({0, 2, 2}));
    EXPECT_FALSE(provided.satisfies({0, 2, 4}));
    EXPECT_FALSE(provided.satisfies({0, 3, 0}));
    EXPECT_FALSE(provided.satisfies({1, 0, 0}));
}

TEST(VersionTest, serializationRoundTrip) {
    const babelwires::Version original{1, 2, 3};
    const std::string serialized = original.serializeToString();
    const auto deserializedResult = babelwires::Version::deserializeFromString(serialized);
    ASSERT_TRUE(deserializedResult.has_value());
    const babelwires::Version deserialized = deserializedResult.value();
    EXPECT_EQ(deserialized.major, original.major);
    EXPECT_EQ(deserialized.minor, original.minor);
    EXPECT_EQ(deserialized.patch, original.patch);
}

TEST(VersionTest, deserializeFromStringAcceptsSemVerWithMetadata) {
    const babelwires::ResultT<babelwires::Version> parsed =
        babelwires::Version::deserializeFromString("1.2.3-alpha.1+build.42");
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->major, 1);
    EXPECT_EQ(parsed->minor, 2);
    EXPECT_EQ(parsed->patch, 3);
}

TEST(VersionTest, deserializeFromStringAcceptsPlainSemVer) {
    const babelwires::ResultT<babelwires::Version> parsed = babelwires::Version::deserializeFromString("0.0.1");
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->major, 0);
    EXPECT_EQ(parsed->minor, 0);
    EXPECT_EQ(parsed->patch, 1);
}

TEST(VersionTest, deserializeFromStringRejectsInvalidSemVerText) {
    EXPECT_FALSE(babelwires::Version::deserializeFromString("01.2.3").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.02.3").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.2.03").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.2").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.2.3-").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.2.3-alpha..1").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.2.3-alpha_1").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("v1.2.3").has_value());
}

TEST(VersionTest, deserializeFromStringRejectsOutOfRangeComponents) {
    EXPECT_FALSE(babelwires::Version::deserializeFromString("65536.0.0").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.65536.0").has_value());
    EXPECT_FALSE(babelwires::Version::deserializeFromString("1.0.65536").has_value());
}
