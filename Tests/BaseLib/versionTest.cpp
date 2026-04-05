#include <gtest/gtest.h>

#include <BaseLib/Version/version.hpp>

TEST(VersionTest, projectVersionNotPlaceholder) {
    const babelwires::Version& v = babelwires::Version::projectVersion();
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
