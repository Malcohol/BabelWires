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

TEST(FixedTest, fromFixedAllowsHigherPrecision) {
    EXPECT_EQ(*Fixed::fromFixed(Fixed(1234, 2), 4), Fixed(123400, 4));
    EXPECT_EQ(*Fixed::fromFixed(Fixed(-1234, 2), 4), Fixed(-123400, 4));
}

TEST(FixedTest, fromFixedRequiresExactValueAtLowerPrecision) {
    EXPECT_TRUE(Fixed::fromFixed(Fixed(1230, 2), 1).has_value());
    EXPECT_FALSE(Fixed::fromFixed(Fixed(1234, 2), 1).has_value());
}

TEST(FixedTest, tryFromFixedRounds) {
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(1230, 2), 1), Fixed(123, 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(1234, 2), 1), Fixed(123, 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(1235, 2), 1), Fixed(124, 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(1236, 2), 1), Fixed(124, 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(-1230, 2), 1), Fixed(-123, 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(-1234, 2), 1), Fixed(-123, 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(-1235, 2), 1), Fixed(-124, 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(-1236, 2), 1), Fixed(-124, 1));
}

TEST(FixedTest, assertFromFixedRequiresExactValue) {
    EXPECT_EQ(Fixed::assertFromFixed(Fixed(1230, 2), 1), Fixed(123, 1));
    EXPECT_DEATH(Fixed::assertFromFixed(Fixed(1234, 2), 1), "");
}

TEST(FixedTest, fromFixedDetectsOverflowWhenIncreasingPrecision) {
    EXPECT_FALSE(Fixed::fromFixed(Fixed(std::numeric_limits<Fixed::NativeType>::max() / 2, 0), 1).has_value());
    EXPECT_FALSE(Fixed::fromFixed(Fixed(std::numeric_limits<Fixed::NativeType>::min() / 2, 0), 1).has_value());
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(std::numeric_limits<Fixed::NativeType>::max() / 2, 0), 1), Fixed(std::numeric_limits<Fixed::NativeType>::max(), 1));
    EXPECT_EQ(Fixed::tryFromFixed(Fixed(std::numeric_limits<Fixed::NativeType>::min() / 2, 0), 1), Fixed(std::numeric_limits<Fixed::NativeType>::min(), 1));
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

TEST(FixedTest, comparisonOperators) {
    Fixed valueA(12, 3);
    Fixed valueB(13, 3);
    Fixed valueAA(12, 3);

    EXPECT_TRUE(valueA < valueB);
    EXPECT_FALSE(valueB < valueA);

    EXPECT_TRUE(valueB > valueA);
    EXPECT_FALSE(valueA > valueB);

    EXPECT_TRUE(valueA <= valueB);
    EXPECT_FALSE(valueB <= valueA);

    EXPECT_TRUE(valueB >= valueA);
    EXPECT_FALSE(valueA >= valueB);

    EXPECT_TRUE(valueA <= valueAA);
    EXPECT_TRUE(valueA >= valueAA);
}

TEST(FixedTest, toString) {
    EXPECT_EQ(Fixed(0, 0).toString(), "0");
    EXPECT_EQ(Fixed(0, 2).toString(), "0");
    EXPECT_EQ(Fixed(10, 0).toString(), "10");
    EXPECT_EQ(Fixed(-10, 0).toString(), "-10");
    EXPECT_EQ(Fixed(1000, 2).toString(), "10");
    EXPECT_EQ(Fixed(-1000, 2).toString(), "-10");
    EXPECT_EQ(Fixed(1010, 2).toString(), "10.1");
    EXPECT_EQ(Fixed(1001, 2).toString(), "10.01");
    EXPECT_EQ(Fixed(-0, 4).toString(), "0");
    EXPECT_EQ(Fixed(-1234, 3).toString(), "-1.234");
    EXPECT_EQ(Fixed(10, 2).toString(), "0.1");
    EXPECT_EQ(Fixed(1, 2).toString(), "0.01");
}

TEST(FixedTest, serializeToString) {
    EXPECT_EQ(Fixed(0, 0).serializeToString(), "0");
    EXPECT_EQ(Fixed(0, 2).serializeToString(), "0.00");
    EXPECT_EQ(Fixed(10, 0).serializeToString(), "10");
    EXPECT_EQ(Fixed(-10, 0).serializeToString(), "-10");
    EXPECT_EQ(Fixed(1000, 2).serializeToString(), "10.00");
    EXPECT_EQ(Fixed(-1000, 2).serializeToString(), "-10.00");
    EXPECT_EQ(Fixed(1010, 2).serializeToString(), "10.10");
    EXPECT_EQ(Fixed(1001, 2).serializeToString(), "10.01");
    EXPECT_EQ(Fixed(-0, 4).serializeToString(), "0.0000");
    EXPECT_EQ(Fixed(-1234, 3).serializeToString(), "-1.234");
    EXPECT_EQ(Fixed(10, 2).serializeToString(), "0.10");
    EXPECT_EQ(Fixed(1, 2).serializeToString(), "0.01");
}

TEST(FixedTest, deserializeFromString) {
    EXPECT_EQ(*Fixed::deserializeFromString("0"), Fixed(0, 0));
    EXPECT_EQ(*Fixed::deserializeFromString("0.0000"), Fixed(0, 4));
    EXPECT_EQ(*Fixed::deserializeFromString("12.34"), Fixed(1234, 2));
    EXPECT_EQ(*Fixed::deserializeFromString("-1.234"), Fixed(-1234, 3));

    EXPECT_FALSE(Fixed::deserializeFromString("a").has_value());
    EXPECT_FALSE(Fixed::deserializeFromString("1..2").has_value());
    EXPECT_FALSE(Fixed::deserializeFromString("1.").has_value());
    EXPECT_FALSE(Fixed::deserializeFromString(".1").has_value());
}

TEST(FixedTest, partialParseNoPrecision) {
    Fixed valueOut;

    EXPECT_EQ(Fixed::partialParse("0", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(0, 0));

    EXPECT_EQ(Fixed::partialParse("0.0000", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(0, 4));

    EXPECT_EQ(Fixed::partialParse("12.34", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(1234, 2));

    EXPECT_EQ(Fixed::partialParse("-1.234", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(-1234, 3));

    EXPECT_EQ(Fixed::partialParse("-", valueOut), Fixed::PartialParseResult::Truncated);
    EXPECT_EQ(Fixed::partialParse("+", valueOut), Fixed::PartialParseResult::Truncated);
    EXPECT_EQ(Fixed::partialParse("1.", valueOut), Fixed::PartialParseResult::Truncated);

    EXPECT_EQ(Fixed::partialParse("1.2.2", valueOut), Fixed::PartialParseResult::Failure);
    EXPECT_EQ(Fixed::partialParse("1..2", valueOut), Fixed::PartialParseResult::Failure);
    EXPECT_EQ(Fixed::partialParse("a", valueOut), Fixed::PartialParseResult::Failure);
    EXPECT_EQ(Fixed::partialParse("1a", valueOut), Fixed::PartialParseResult::Failure);
    EXPECT_EQ(Fixed::partialParse("1.2a", valueOut), Fixed::PartialParseResult::Failure);

    // Overflow
    // int64_t Min = -9,223,372,036,854,775,808
    // int64_t Max = 9,223,372,036,854,775,807
    EXPECT_EQ(Fixed::partialParse("-9223372036854775808", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(Fixed::partialParse("-9223372036854775809", valueOut), Fixed::PartialParseResult::Failure);
    EXPECT_EQ(Fixed::partialParse("-9999999999999999999", valueOut), Fixed::PartialParseResult::Failure);
    EXPECT_EQ(Fixed::partialParse("9223372036854775807", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(Fixed::partialParse("9223372036854775808", valueOut), Fixed::PartialParseResult::Failure);
    EXPECT_EQ(Fixed::partialParse("9999999999999999999", valueOut), Fixed::PartialParseResult::Failure);

    EXPECT_EQ(Fixed::partialParse("-9223372036854.775808", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(Fixed::partialParse("-9223372036854.775809999", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(Fixed::partialParse("9223372036854.775807", valueOut), Fixed::PartialParseResult::Success);
    EXPECT_EQ(Fixed::partialParse("9223372036854.775808999", valueOut), Fixed::PartialParseResult::Success);
}

TEST(FixedTest, partialParseWithPrecision) {
    Fixed valueOut;

    EXPECT_EQ(Fixed::partialParse("14", valueOut, 0), Fixed::PartialParseResult::Success);
    
    EXPECT_EQ(Fixed::partialParse("-", valueOut, 2), Fixed::PartialParseResult::Truncated);
    EXPECT_EQ(Fixed::partialParse("+", valueOut, 2), Fixed::PartialParseResult::Truncated);

    EXPECT_EQ(Fixed::partialParse("1", valueOut, 2), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(100, 2));
    EXPECT_EQ(Fixed::partialParse("14", valueOut, 2), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(1400, 2));
    EXPECT_EQ(Fixed::partialParse("14.", valueOut, 2), Fixed::PartialParseResult::Truncated);
    EXPECT_EQ(Fixed::partialParse("14.2", valueOut, 2), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(1420, 2));
    EXPECT_EQ(Fixed::partialParse("14.23", valueOut, 2), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(1423, 2));
    EXPECT_EQ(Fixed::partialParse("14.235", valueOut, 2), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(1424, 2));

    EXPECT_EQ(Fixed::partialParse("-14.235", valueOut, 2), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(-1424, 2));

    // Too much precision
    EXPECT_EQ(Fixed::partialParse("14.555555555555555555555", valueOut, 3), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(14556, 3));

    // Overflow (clamps)
    // int64_t Min = -9,223,372,036,854,775,808
    // int64_t Max = 9,223,372,036,854,775,807
    EXPECT_EQ(Fixed::partialParse("-9223372036854775808", valueOut, 0), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::min(), 0));

    EXPECT_EQ(Fixed::partialParse("-9223372036854775809", valueOut, 0), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::min(), 0));

    EXPECT_EQ(Fixed::partialParse("-9999999999999999999", valueOut, 0), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::min(), 0));

    EXPECT_EQ(Fixed::partialParse("9223372036854775807", valueOut, 0), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::max(), 0));

    EXPECT_EQ(Fixed::partialParse("9223372036854775808", valueOut, 0), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::max(), 0));

    EXPECT_EQ(Fixed::partialParse("9999999999999999999", valueOut, 0), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::max(), 0));

    EXPECT_EQ(Fixed::partialParse("-9223372036854.775808", valueOut, 6), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::min(), 6));

    EXPECT_EQ(Fixed::partialParse("-9223372036854.775809999", valueOut, 6), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::min(), 6));

    EXPECT_EQ(Fixed::partialParse("9223372036854.775807", valueOut, 6), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::max(), 6));

    EXPECT_EQ(Fixed::partialParse("9223372036854.775808999", valueOut, 6), Fixed::PartialParseResult::Success);
    EXPECT_EQ(valueOut, Fixed(std::numeric_limits<Fixed::NativeType>::max(), 6));
}
