#include <BaseLib/Math/fixed.hpp>

#include <gtest/gtest.h>

#include <unordered_set>

using namespace babelwires;

TEST(FixedTest, basics) {
    Fixed fixedValue;
    EXPECT_EQ(fixedValue.getNumerator(), 0);
    EXPECT_EQ(fixedValue.getPrecision(), 0);

    fixedValue = Fixed(123, 2);
    EXPECT_EQ(fixedValue.getNumerator(), 123);
    EXPECT_EQ(fixedValue.getPrecision(), 2);

    fixedValue = Fixed(-0, 3);
    EXPECT_EQ(fixedValue.getNumerator(), 0);
    EXPECT_EQ(fixedValue.getPrecision(), 3);
}

TEST(FixedTest, equalityAndHash) {
    Fixed valueA(12, 3);
    Fixed valueB(12, 3);
    Fixed valueC(12, 2);

    EXPECT_EQ(valueA, valueB);
    EXPECT_NE(valueA, valueC);
    EXPECT_EQ(valueA.getHash(), valueB.getHash());
    EXPECT_NE(valueA.getHash(), valueC.getHash());

    std::unordered_set<Fixed> set;
    EXPECT_EQ(set.find(Fixed(1, 2)), set.end());
    EXPECT_EQ(set.insert(Fixed(1, 2)).second, true);
    EXPECT_EQ(set.insert(Fixed(1, 2)).second, false);
    EXPECT_NE(set.find(Fixed(1, 2)), set.end());
}

TEST(FixedTest, stringConversion) {
    EXPECT_EQ(Fixed(-1234, 3).toString(), "-1.234");
    EXPECT_EQ(Fixed(-0, 4).toString(), "0.0000");
    EXPECT_EQ(Fixed(0, 0).toString(), "0");
    EXPECT_EQ(Fixed(42, 2).toString(), "0.42");
}

TEST(FixedTest, deserializeFromString) {
    EXPECT_EQ(*Fixed::deserializeFromString("0"), Fixed(0, 0));
    EXPECT_EQ(*Fixed::deserializeFromString("12.34"), Fixed(1234, 2));
    EXPECT_EQ(*Fixed::deserializeFromString("-1.234"), Fixed(-1234, 3));

    EXPECT_FALSE(Fixed::deserializeFromString("a").has_value());
    EXPECT_FALSE(Fixed::deserializeFromString("1..2").has_value());
    EXPECT_FALSE(Fixed::deserializeFromString("1.").has_value());
    EXPECT_FALSE(Fixed::deserializeFromString(".1").has_value());
}
