#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(RationalTypeTest, defaultRationalTypeCreateValue) {
    babelwires::DefaultRationalType rationalType;

    babelwires::ValueHolder newValue = rationalType.createValue();
    EXPECT_TRUE(newValue);

    const auto* const newRationalValue = newValue->as<babelwires::RationalValue>();
    EXPECT_NE(newRationalValue, nullptr);
    EXPECT_EQ(newRationalValue->get(), 0);
}

TEST(RationalTypeTest, defaultRationalTypeGetRange) {
    babelwires::DefaultRationalType rationalType;

    auto range = rationalType.getRange();

    EXPECT_EQ(range.m_min, std::numeric_limits<babelwires::Rational>::min());
    EXPECT_EQ(range.m_max, std::numeric_limits<babelwires::Rational>::max());
}

TEST(RationalTypeTest, defaultRationalTypeIsValidValue) {
    babelwires::DefaultRationalType rationalType;

    babelwires::RationalValue value(babelwires::Rational(1 / 3));

    EXPECT_TRUE(rationalType.isValidValue(value));

    const babelwires::RationalValue minValue = std::numeric_limits<babelwires::Rational>::min();
    const babelwires::RationalValue maxValue = std::numeric_limits<babelwires::Rational>::max();

    EXPECT_TRUE(rationalType.isValidValue(minValue));
    EXPECT_TRUE(rationalType.isValidValue(maxValue));

    EXPECT_FALSE(rationalType.isValidValue(babelwires::StringValue("Hello")));
    EXPECT_FALSE(rationalType.isValidValue(babelwires::IntValue(3)));
}

TEST(RationalTypeTest, defaultRationalTypeGetKind) {
    babelwires::DefaultRationalType rationalType;

    EXPECT_FALSE(rationalType.getKind().empty());
}

TEST(RationalTypeTest, defaultRationalTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const auto* const foundType =
        testEnvironment.m_typeSystem.tryGetPrimitiveType(babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_NE(foundType, nullptr);
    EXPECT_NE(foundType->as<babelwires::DefaultRationalType>(), nullptr);
}

TEST(RationalTypeTest, constructedRationalTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef rationalTypeRef(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    const babelwires::Type* const rationalType = rationalTypeRef.tryResolve(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = rationalType->createValue();
    EXPECT_TRUE(newValue);

    const auto* const newRationalValue = newValue->as<babelwires::RationalValue>();
    EXPECT_NE(newRationalValue, nullptr);
    EXPECT_EQ(newRationalValue->get(), 1);
}

TEST(RationalTypeTest, constructedRationalTypeRange) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef rationalTypeRef(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    const babelwires::Type* const type = rationalTypeRef.tryResolve(testEnvironment.m_typeSystem);

    const babelwires::RationalType* const rationalType = type->as<babelwires::RationalType>();
    ASSERT_NE(rationalType, nullptr);

    auto range = rationalType->getRange();
    EXPECT_EQ(range.m_min, babelwires::Rational(2, 3));
    EXPECT_EQ(range.m_max, babelwires::Rational(4, 3));
}

TEST(RationalTypeTest, constructedRationalTypeIsValidValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef rationalTypeRef(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    const babelwires::Type* const type = rationalTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_FALSE(type->isValidValue(babelwires::RationalValue(babelwires::Rational(1, 2))));
    EXPECT_TRUE(type->isValidValue(babelwires::RationalValue(babelwires::Rational(2, 3))));
    EXPECT_TRUE(type->isValidValue(babelwires::RationalValue(babelwires::Rational(4, 3))));
    EXPECT_FALSE(type->isValidValue(babelwires::RationalValue(babelwires::Rational(3, 2))));

    EXPECT_FALSE(type->isValidValue(babelwires::StringValue("Hello")));
    EXPECT_FALSE(type->isValidValue(babelwires::IntValue(3)));
}

TEST(RationalTypeTest, sameKind) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef rationalTypeRef(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    const babelwires::Type* const type = rationalTypeRef.tryResolve(testEnvironment.m_typeSystem);

    babelwires::DefaultRationalType defaultRationalType;

    EXPECT_EQ(defaultRationalType.getKind(), type->getKind());
}