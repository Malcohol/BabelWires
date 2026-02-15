#include <BaseLib/Math/rational.hpp>
#include <BaseLib/exceptions.hpp>

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

TEST(RationalTest, deserializeFromString) {
    // Success
    EXPECT_EQ(*Rational::deserializeFromString("0"), Rational(0));
    EXPECT_EQ(*Rational::deserializeFromString("1"), Rational(1));
    EXPECT_EQ(*Rational::deserializeFromString("-1"), Rational(-1));
    EXPECT_EQ(*Rational::deserializeFromString("-1/2"), Rational(-1, 2));

    EXPECT_FALSE(Rational::deserializeFromString("a").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("--1").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("01").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("1/").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("1/a").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("1a").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("1/02").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("1/2/3").has_value());
    EXPECT_FALSE(Rational::deserializeFromString("1/-3").has_value());
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

TEST(RationalTest, divmod) {
    // Positive
    {
        const auto divmod = Rational(1).divmod(Rational(1));
        EXPECT_EQ(std::get<0>(divmod), 1);    
        EXPECT_EQ(std::get<1>(divmod), 0);
    }
    {
        const auto divmod = Rational(1).divmod(Rational(2));
        EXPECT_EQ(std::get<0>(divmod), 0);
        EXPECT_EQ(std::get<1>(divmod), 1);
    }
    {
        const auto divmod = Rational(1).divmod(Rational(1, 2));
        EXPECT_EQ(std::get<0>(divmod), 2);    
        EXPECT_EQ(std::get<1>(divmod), 0);
    }
    {
        const auto divmod = Rational(3, 2).divmod(Rational(1));
        EXPECT_EQ(std::get<0>(divmod), 1); 
        EXPECT_EQ(std::get<1>(divmod), Rational(1, 2));
    }
    {
        const auto divmod = Rational(1, 2).divmod(Rational(1, 3));
        EXPECT_EQ(std::get<0>(divmod), 1); 
        EXPECT_EQ(std::get<1>(divmod), Rational(1, 6));
    }
    {
        const auto divmod = Rational(7, 2).divmod(Rational(1, 3));
        EXPECT_EQ(std::get<0>(divmod), 10); 
        EXPECT_EQ(std::get<1>(divmod), Rational(1, 6));
    }
    // Negative
    {
        const auto divmod = Rational(-2, 3).divmod(Rational(1, 2));
        EXPECT_EQ(std::get<0>(divmod), -1); 
        EXPECT_EQ(std::get<1>(divmod), -Rational(1, 6));
    }
}

TEST(RationalTest, continuedFractionApproximation) {
    using ComponentType = Rational::ComponentType;
    const ComponentType maxComp = std::numeric_limits<ComponentType>::max();

    // Helper: check that a result's value (as double) is close to expected.
    auto expectClose = [](Rational result, double expected, double relTol = 1e-8) {
        double actual = static_cast<double>(result.getNumerator()) /
                        static_cast<double>(result.getDenominator());
        EXPECT_NEAR(actual, expected, std::abs(expected) * relTol);
    };

    // GCD reduction avoids approximation entirely.
    // setComponents(2 * maxComp, 3 * maxComp) reduces to 2/3 exactly.
    {
        Rational result = Rational(maxComp, 3) * Rational(2, maxComp);
        EXPECT_EQ(result, Rational(2, 3));
    }

    // Denominator overflow, known exact result.
    // Rational(3, 7) * Rational(maxComp, maxComp - 1) calls
    // setComponents(3 * maxComp, 7 * (maxComp - 1)).
    // After GCD reduction by 3: setComponents(maxComp, 5010795174).
    // The CF converges to 920350134 / 2147483645 (verified by hand-tracing).
    {
        Rational result = Rational(3, 7) * Rational(maxComp, maxComp - 1);
        EXPECT_EQ(result, Rational(920350134, 2147483645));
        expectClose(result, 3.0 / 7.0, 1e-9);
    }

    // Same as above but negative: sign should be preserved.
    {
        Rational result = Rational(-3, 7) * Rational(maxComp, maxComp - 1);
        EXPECT_EQ(result, Rational(-920350134, 2147483645));
        EXPECT_LT(result.getNumerator(), 0);
        EXPECT_GT(result.getDenominator(), 0);
    }

    // Large numerator with small denominator (exercises the p-bound on k).
    // Rational(maxComp, 3) + Rational(maxComp, 5) calls
    // setComponents(8 * maxComp, 15). After GCD = 1: 17179869176 / 15.
    // CF: [1145324611; 1, 2, ...]. The convergent 1145324612/1 fits;
    // the next one (3435973835/3) does not, and the p-bound forces k=0
    // so the result stays at 1145324612.
    {
        Rational result = Rational(maxComp, 3) + Rational(maxComp, 5);
        EXPECT_EQ(result, Rational(1145324612));
        expectClose(result, 8.0 * maxComp / 15.0, 1e-6);
    }

    // Integer overflow: value exceeds maxComp (exercises the q1=0 path).
    // Rational(maxComp, 1) * Rational(2, 1) calls setComponents(2 * maxComp, 1).
    // The integer part (2 * maxComp) exceeds maxComp on the first CF iteration.
    // Best possible approximation: clamp to maxComp / 1.
    {
        Rational result = Rational(maxComp, 1) * Rational(2, 1);
        EXPECT_EQ(result, Rational(maxComp));
    }

    // Squaring values near sqrt(maxComp) causes both numerator and
    // denominator to overflow. 46341^2 = 2147486281 > maxComp,
    // 46340^2 = 2147393600 < maxComp. True value ≈ 1.0000432.
    {
        Rational result = Rational(46341, 46340) * Rational(46341, 46340);
        EXPECT_GT(result.getNumerator(), 0);
        EXPECT_GT(result.getDenominator(), 0);
        EXPECT_LE(result.getNumerator(), maxComp);
        EXPECT_LE(result.getDenominator(), maxComp);
        double trueValue = (46341.0 / 46340.0) * (46341.0 / 46340.0);
        expectClose(result, trueValue, 1e-8);
    }

    // Both components massively overflow (product of two large primes).
    // 2147483629 and 2147483587 are primes, so no GCD reduction occurs.
    // Result should be close to their ratio ≈ 1.0000000196.
    {
        Rational result = Rational(2147483629, 2147483587) *
                          Rational(2147483629, 2147483587);
        EXPECT_GT(result.getDenominator(), 0);
        EXPECT_LE(result.getNumerator(), maxComp);
        EXPECT_LE(result.getDenominator(), maxComp);
        double trueValue = (2147483629.0 / 2147483587.0) *
                           (2147483629.0 / 2147483587.0);
        expectClose(result, trueValue, 1e-6);
    }

    // Division creating extreme values: (maxComp-1)*(maxComp-2) / 6
    // is far beyond maxComp, so the value clamps to maxComp / 1.
    {
        Rational result = Rational(maxComp - 1, 2) / Rational(3, maxComp - 2);
        EXPECT_EQ(result, Rational(maxComp));
    }
}