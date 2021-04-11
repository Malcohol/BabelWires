#include "Common/Utilities/enumFlags.hpp"
#include <gtest/gtest.h>

namespace {
    enum class TestEnum : unsigned short { red = 0b001, big = 0b010, fast = 0b100 };

    DEFINE_ENUM_FLAG_OPERATORS(TestEnum);
} // namespace

TEST(EnumFlagsTest, operators) {
    EXPECT_EQ(TestEnum::red | TestEnum::big, static_cast<TestEnum>(0b011));
    EXPECT_EQ(TestEnum::red & static_cast<TestEnum>(0b011), TestEnum::red);
    EXPECT_EQ(TestEnum::red & TestEnum::fast, static_cast<TestEnum>(0));
    EXPECT_EQ(~(TestEnum::red | TestEnum::big) & TestEnum::red, static_cast<TestEnum>(0));
    EXPECT_EQ(~(TestEnum::red | TestEnum::big) & TestEnum::big, static_cast<TestEnum>(0));
    EXPECT_EQ(~(TestEnum::red | TestEnum::big) & TestEnum::fast, TestEnum::fast);
    EXPECT_EQ(~static_cast<TestEnum>(0) & TestEnum::fast, TestEnum::fast);
    EXPECT_TRUE(isNonzero(TestEnum::red));
    EXPECT_FALSE(isNonzero(static_cast<TestEnum>(0)));
    EXPECT_TRUE(isZero(static_cast<TestEnum>(0)));
    EXPECT_FALSE(isZero(TestEnum::big));
}
