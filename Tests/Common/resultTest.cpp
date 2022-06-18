#include <gtest/gtest.h>

#include <Common/Utilities/result.hpp>

TEST(ResultTest, success) {
    babelwires::Result result(babelwires::Result::success);
    EXPECT_TRUE(result);
}

TEST(ResultTest, failureString) {
    babelwires::Result result(std::string("Ouch"));
    EXPECT_FALSE(result);
    EXPECT_EQ(result.getReasonWhyFailed(), "Ouch");
}

TEST(ResultTest, failureLiteral) {
    babelwires::Result result("Ouch");
    EXPECT_FALSE(result);
    EXPECT_EQ(result.getReasonWhyFailed(), "Ouch");
}