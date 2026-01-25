#include <gtest/gtest.h>

#include <BaseLib/Utilities/result.hpp>

TEST(ResultTest, success) {
    babelwires::Result result{};
    EXPECT_TRUE(result);
}

TEST(ResultTest, failureString) {
    babelwires::Result result = std::unexpected(babelwires::ErrorStorage(std::string("Ouch")));
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().toString(), "Ouch");
}

TEST(ResultTest, failureLiteral) {
    babelwires::Result result = std::unexpected(babelwires::ErrorStorage("Ouch"));
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().toString(), "Ouch");
}