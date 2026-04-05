#include <gtest/gtest.h>

#include <BaseLib/BuildCompatibility/buildCompatibility.hpp>
#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>

#include <cstring>
#include <string>

TEST(BuildCompatibilityTest, compareEqualFingerprints) {
    char expected[babelwires::c_buildFingerprintBufferSize] = {};
    char actual[babelwires::c_buildFingerprintBufferSize] = {};

    const std::size_t expectedSize = babelwires::writeBuildFingerprint(expected, sizeof(expected));
    const std::size_t actualSize = babelwires::writeBuildFingerprint(actual, sizeof(actual));

    EXPECT_GT(expectedSize, 0u);
    EXPECT_EQ(expectedSize, actualSize);

    const std::string expectedFingerprint(expected, std::strlen(expected));
    const std::string actualFingerprint(actual, std::strlen(actual));

    const babelwires::Result result = babelwires::compareBuildFingerprints(expectedFingerprint, actualFingerprint);
    EXPECT_TRUE(result);
}

TEST(BuildCompatibilityTest, compareMismatchIncludesBothFingerprints) {
    const std::string expectedFingerprint = "schema_version=1\ncompiler=gcc\n";
    const std::string actualFingerprint = "schema_version=1\ncompiler=clang\n";

    const babelwires::Result result = babelwires::compareBuildFingerprints(expectedFingerprint, actualFingerprint);
    EXPECT_FALSE(result);

    const std::string errorText = result.error().toString();
    EXPECT_NE(errorText.find("Build fingerprints do not match."), std::string::npos);
    EXPECT_NE(errorText.find("Expected:\n" + expectedFingerprint), std::string::npos);
    EXPECT_NE(errorText.find("Actual:\n" + actualFingerprint), std::string::npos);
}

TEST(BuildCompatibilityTest, writeBuildFingerprintTruncatesWithNullTerminator) {
    char smallBuffer[32];
    std::memset(smallBuffer, 'X', sizeof(smallBuffer));

    const std::size_t bytesUsed = babelwires::writeBuildFingerprint(smallBuffer, sizeof(smallBuffer));

    EXPECT_EQ(bytesUsed, sizeof(smallBuffer));
    EXPECT_EQ(smallBuffer[sizeof(smallBuffer) - 1], '\0');

    const std::string truncatedFingerprint(smallBuffer, std::strlen(smallBuffer));
    EXPECT_EQ(truncatedFingerprint.find("schema_version=1\n"), 0u);
}
