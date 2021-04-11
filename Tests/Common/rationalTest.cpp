#include "Common/Math/rational.hpp"
#include "Common/exceptions.hpp"
#include <gtest/gtest.h>

#include <unordered_set>

using namespace babelwires;

TEST(RationalTest, basicOperations) {
    EXPECT_EQ(Rational(1, 2) + Rational(1, 2), Rational(1));
    EXPECT_EQ(Rational(1, 2) - Rational(1, 2), Rational(0));
    EXPECT_EQ(Rational(2, 4), Rational(1, 2));
    EXPECT_EQ(Rational(1, 2) + Rational(2, 3), Rational(7, 6));
    EXPECT_EQ(Rational(1, 2) * Rational(2, 3), Rational(1, 3));
    EXPECT_EQ(Rational(3, 2) * Rational(2, 3), Rational(1));
    EXPECT_EQ(Rational(1, 2) / Rational(2, 3), Rational(3, 4));
    EXPECT_EQ(-Rational(1, 2), Rational(-1, 2));
    EXPECT_EQ(-Rational(1, 2), Rational(1, -2));
    EXPECT_EQ(-Rational(1, 2) + Rational(1, 2), Rational(0));
    EXPECT_EQ(Rational(-1, -2), Rational(1, 2));
    EXPECT_EQ(Rational(0, 1), Rational(0));
    EXPECT_EQ(Rational(0, -1), Rational(0));
    EXPECT_EQ(Rational(-0, 1), Rational(0));
}

TEST(RationalTest, nearLimits) {
    Rational::ComponentType bigEvenVal = std::numeric_limits<Rational::ComponentType>::max() - 1;
    ASSERT_EQ(bigEvenVal % 2, 0);
    EXPECT_EQ(Rational(std::numeric_limits<Rational::ComponentType>::max(),
                       std::numeric_limits<Rational::ComponentType>::max()),
              Rational(1));

    Rational bigPos{bigEvenVal, 1};
    Rational bigNeg{-bigEvenVal, 1};
    Rational tiny{1, bigEvenVal};
    Rational justLessThanOne{bigEvenVal - 1, bigEvenVal};
    Rational justMoreThanOne{bigEvenVal, bigEvenVal - 1};
    Rational halfBigPos{bigEvenVal / 2, 1};
    Rational halfAsTiny{1, bigEvenVal / 2};

    EXPECT_EQ(bigPos + bigNeg, Rational());
    EXPECT_EQ(bigPos / bigPos, Rational(1));
    EXPECT_EQ(bigNeg / bigNeg, Rational(1));
    EXPECT_EQ(Rational(1) / bigPos, tiny);
    EXPECT_EQ(bigPos * tiny, Rational(1));
    EXPECT_EQ(justLessThanOne * justMoreThanOne, Rational(1));
    EXPECT_EQ(halfBigPos + halfBigPos, bigPos);
    EXPECT_EQ(halfBigPos * 2, bigPos);
    EXPECT_EQ(tiny * 2, halfAsTiny);
}

TEST(RationalTest, boolOperators) {
    EXPECT_TRUE(Rational(1, 2) == Rational(50, 100));
    EXPECT_FALSE(Rational(1, 2) != Rational(50, 100));
    EXPECT_FALSE(Rational(1, 2) == Rational(3, 2));
    EXPECT_TRUE(Rational(1, 2) != Rational(3, 2));

    EXPECT_TRUE(Rational(1, 2) <= Rational(1, 2));
    EXPECT_TRUE(Rational(1, 2) >= Rational(1, 2));

    EXPECT_TRUE(Rational(1, 2) < Rational(3, 2));
    EXPECT_TRUE(Rational(1, 2) <= Rational(3, 2));
    EXPECT_TRUE(Rational(-1, 2) < Rational(1, 4));
    EXPECT_TRUE(Rational(-1, 2) <= Rational(1, 4));
    EXPECT_TRUE(Rational(-1, 2) < Rational(-1, 4));
    EXPECT_TRUE(Rational(-1, 2) <= Rational(-1, 4));

    EXPECT_TRUE(Rational(3, 2) > Rational(1, 2));
    EXPECT_TRUE(Rational(3, 2) >= Rational(2, 2));
    EXPECT_TRUE(Rational(1, 4) > Rational(-1, 2));
    EXPECT_TRUE(Rational(1, 4) >= Rational(-1, 2));
    EXPECT_TRUE(Rational(-1, 4) > Rational(-1, 2));
    EXPECT_TRUE(Rational(-1, 4) >= Rational(-1, 2));
}

TEST(RationalTest, toString) {
    EXPECT_EQ(Rational(1, 2).toString(), "1/2");
    EXPECT_EQ(Rational(3, 2).toString(), "1 1/2");
    EXPECT_EQ(Rational(-1, 2).toString(), "-1/2");
    EXPECT_EQ(Rational(-3, 2).toString(), "-1 1/2");
    EXPECT_EQ(Rational(5, 5).toString(), "1");
    EXPECT_EQ(Rational(0, 5).toString(), "0");
}

TEST(RationalTest, toHtmlString) {
    EXPECT_EQ(Rational(1, 2).toHtmlString(), "<sup>1</sup>&frasl;<sub>2</sub>");
    EXPECT_EQ(Rational(3, 2).toHtmlString(), "1<sup>1</sup>&frasl;<sub>2</sub>");
    EXPECT_EQ(Rational(-1, 2).toHtmlString(), "<sup>-1</sup>&frasl;<sub>2</sub>");
    EXPECT_EQ(Rational(-3, 2).toHtmlString(), "-1<sup>1</sup>&frasl;<sub>2</sub>");
    EXPECT_EQ(Rational(5, 5).toHtmlString(), "1");
    EXPECT_EQ(Rational(0, 5).toHtmlString(), "0");
}

TEST(RationalTest, partialParse) {
    // Success.
    Rational r;
    EXPECT_EQ(Rational::partialParse("", r), Rational::PartialParseResult::Truncated);
    EXPECT_EQ(Rational::partialParse("a", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("0", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(0));
    EXPECT_EQ(Rational::partialParse("01", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("1", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(1));
    EXPECT_EQ(Rational::partialParse("1a", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("1/", r), Rational::PartialParseResult::Truncated);
    EXPECT_EQ(Rational::partialParse("1/-", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("1/a", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("1/0", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("1/2", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(1, 2));
    EXPECT_EQ(Rational::partialParse("11", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(11));
    EXPECT_EQ(Rational::partialParse("11/", r), Rational::PartialParseResult::Truncated);
    EXPECT_EQ(Rational::partialParse("11/2", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(11, 2));
    EXPECT_EQ(Rational::partialParse("11/20", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(11, 20));
    EXPECT_EQ(Rational::partialParse("11/20 ", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(11, 20));
    EXPECT_EQ(Rational::partialParse("11/20a", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("11/20 1", r), Rational::PartialParseResult::Failure);

    EXPECT_EQ(Rational::partialParse("-1", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(-1));
    EXPECT_EQ(Rational::partialParse("- 1", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(-1));
    EXPECT_EQ(Rational::partialParse("-1/", r), Rational::PartialParseResult::Truncated);
    EXPECT_EQ(Rational::partialParse("-1/2", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(-1, 2));
    EXPECT_EQ(Rational::partialParse("-1/-2", r), Rational::PartialParseResult::Failure);

    EXPECT_EQ(Rational::partialParse("-1 1", r), Rational::PartialParseResult::Truncated);
    EXPECT_EQ(Rational::partialParse("-1 -", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("-1 a", r), Rational::PartialParseResult::Failure);
    EXPECT_EQ(Rational::partialParse("-1 1/", r), Rational::PartialParseResult::Truncated);
    EXPECT_EQ(Rational::partialParse("-1 1/2", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(r, Rational(-3, 2));

    EXPECT_EQ(Rational::partialParse("-0", r), Rational::PartialParseResult::Success);
    EXPECT_EQ(Rational::partialParse("0/", r), Rational::PartialParseResult::Truncated);
}

TEST(RationalTest, parseString) {
    // Success
    EXPECT_EQ(Rational::parseString("0"), Rational(0));
    EXPECT_EQ(Rational::parseString("1"), Rational(1));
    EXPECT_EQ(Rational::parseString("-1"), Rational(-1));
    EXPECT_EQ(Rational::parseString("-1/2"), Rational(-1, 2));

    EXPECT_THROW(Rational::parseString("a"), ParseException);
    EXPECT_THROW(Rational::parseString("--1"), ParseException);
    EXPECT_THROW(Rational::parseString("01"), ParseException);
    EXPECT_THROW(Rational::parseString("1/"), ParseException);
    EXPECT_THROW(Rational::parseString("1/a"), ParseException);
    EXPECT_THROW(Rational::parseString("1a"), ParseException);
    EXPECT_THROW(Rational::parseString("1/02"), ParseException);
    EXPECT_THROW(Rational::parseString("1/2/3"), ParseException);
    EXPECT_THROW(Rational::parseString("1/-3"), ParseException);
}

TEST(RationalTest, serializeDeserialize) {
    const auto serializeAndDeserialize = [](Rational r) {
        return Rational::deserializeFromString(r.serializeToString()) == r;
    };

    EXPECT_TRUE(serializeAndDeserialize(Rational(0)));
    EXPECT_TRUE(serializeAndDeserialize(Rational(1)));
    EXPECT_TRUE(serializeAndDeserialize(Rational(1, 2)));
    EXPECT_TRUE(serializeAndDeserialize(Rational(-1, 2)));
    EXPECT_TRUE(serializeAndDeserialize(Rational(3, 2)));
    EXPECT_TRUE(serializeAndDeserialize(Rational(-3, 2)));
}

TEST(RationalTest, hash) {
    std::unordered_set<Rational> set;
    EXPECT_EQ(set.find(Rational(1)), set.end());
    EXPECT_EQ(set.insert(Rational(1)).second, true);
    EXPECT_EQ(set.insert(Rational(1)).second, false);
    EXPECT_NE(set.find(Rational(1)), set.end());
    EXPECT_EQ(set.find(Rational(1, 2)), set.end());
    EXPECT_EQ(set.insert(Rational(1, 2)).second, true);
    EXPECT_NE(set.find(Rational(1, 2)), set.end());
    EXPECT_NE(set.find(Rational(1, 2)), set.find(Rational(1)));
}
