#include <gtest/gtest.h>

#include <BaseLib/types.hpp>

TEST(TypesTest, isValidIdentifier) {
    EXPECT_TRUE(babelwires::isValidIdentifier("a"));
    EXPECT_TRUE(babelwires::isValidIdentifier("abcde"));
    EXPECT_TRUE(babelwires::isValidIdentifier("abcde0"));
    EXPECT_TRUE(babelwires::isValidIdentifier("ABCDE"));
    EXPECT_TRUE(babelwires::isValidIdentifier("ABCDE0"));
    EXPECT_TRUE(babelwires::isValidIdentifier("ABCDEabcde"));
    EXPECT_TRUE(babelwires::isValidIdentifier("_"));
    EXPECT_TRUE(babelwires::isValidIdentifier("______"));
    EXPECT_TRUE(babelwires::isValidIdentifier("__4__"));
    EXPECT_TRUE(babelwires::isValidIdentifier("_abc"));
    EXPECT_TRUE(babelwires::isValidIdentifier("ab_CD_0"));
    EXPECT_TRUE(babelwires::isValidIdentifier("abcdefghijklmnopqrstuvwxyz_0123456789"));

    EXPECT_FALSE(babelwires::isValidIdentifier(""));
    EXPECT_FALSE(babelwires::isValidIdentifier("abcde "));
    EXPECT_FALSE(babelwires::isValidIdentifier(" abcde"));
    EXPECT_FALSE(babelwires::isValidIdentifier("abc de"));
    EXPECT_FALSE(babelwires::isValidIdentifier("0"));
    EXPECT_FALSE(babelwires::isValidIdentifier("100"));
    EXPECT_FALSE(babelwires::isValidIdentifier("0abc"));
}

TEST(TypesTest, range) {
    EXPECT_TRUE(babelwires::Range<int>(-2, 2).contains(-2));
    EXPECT_TRUE(babelwires::Range<int>(-2, 2).contains(2));
    EXPECT_TRUE(babelwires::Range<int>(-2, 2).contains(1));
    EXPECT_TRUE(babelwires::Range<int>(-2, 2).contains(0));
    EXPECT_TRUE(babelwires::Range<int>(-2, 2).contains(1));
    EXPECT_TRUE(babelwires::Range<int>(-2, 2).contains(2));

    EXPECT_TRUE(babelwires::Range<int>().contains(0));
    EXPECT_FALSE(babelwires::Range<int>(44).contains(0));
    EXPECT_TRUE(babelwires::Range<int>(44).contains(44));
    EXPECT_TRUE(babelwires::Range<int>(44).contains(144));
    EXPECT_TRUE(babelwires::Range<int>(-44).contains(0));

    EXPECT_TRUE(babelwires::Range<unsigned int>(0, 5).contains(0));
    EXPECT_TRUE(babelwires::Range<unsigned int>(0, 5).contains(1));
    EXPECT_TRUE(babelwires::Range<unsigned int>(0, 5).contains(2));
    EXPECT_TRUE(babelwires::Range<unsigned int>(0, 5).contains(3));
    EXPECT_TRUE(babelwires::Range<unsigned int>(0, 5).contains(4));
    EXPECT_TRUE(babelwires::Range<unsigned int>(0, 5).contains(5));

    EXPECT_TRUE(babelwires::Range<unsigned int>().contains(0));
    EXPECT_FALSE(babelwires::Range<unsigned int>(44).contains(0));
    EXPECT_TRUE(babelwires::Range<unsigned int>(44).contains(44));
    EXPECT_TRUE(babelwires::Range<unsigned int>(44).contains(144));
}

// Some tests for the CopyConst template. I could put these in types.cpp, but they'd be clutter there.
static_assert(std::is_same_v<babelwires::CopyConst<const int, float>::type, const float>);
static_assert(std::is_same_v<babelwires::CopyConst<int, float>::type, float>);