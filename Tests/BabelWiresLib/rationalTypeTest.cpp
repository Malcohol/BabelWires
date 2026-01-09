#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(RationalTypeTest, defaultRationalTypeCreateValue) {
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultRationalType rationalType;

    babelwires::ValueHolder newValue = rationalType.createValue(typeSystem);
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
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultRationalType rationalType;
    
    babelwires::RationalValue value(babelwires::Rational(1 / 3));

    EXPECT_TRUE(rationalType.isValidValue(typeSystem, value));

    const babelwires::RationalValue minValue = std::numeric_limits<babelwires::Rational>::min();
    const babelwires::RationalValue maxValue = std::numeric_limits<babelwires::Rational>::max();

    EXPECT_TRUE(rationalType.isValidValue(typeSystem, minValue));
    EXPECT_TRUE(rationalType.isValidValue(typeSystem, maxValue));

    EXPECT_FALSE(rationalType.isValidValue(typeSystem, babelwires::StringValue("Hello")));
    EXPECT_FALSE(rationalType.isValidValue(typeSystem, babelwires::IntValue(3)));
}

TEST(RationalTypeTest, defaultRationalTypeGetKind) {
    babelwires::DefaultRationalType rationalType;

    EXPECT_FALSE(rationalType.getFlavour().empty());
}

TEST(RationalTypeTest, defaultRationalTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredType(babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->as<babelwires::DefaultRationalType>(), nullptr);
}

TEST(RationalTypeTest, constructedRationalTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp rationalTypeExp(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    babelwires::TypePtr rationalType = rationalTypeExp.tryResolve(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = rationalType->createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newRationalValue = newValue->as<babelwires::RationalValue>();
    EXPECT_NE(newRationalValue, nullptr);
    EXPECT_EQ(newRationalValue->get(), 1);
}

TEST(RationalTypeTest, constructedRationalTypeRange) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp rationalTypeExp(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    babelwires::TypePtr type = rationalTypeExp.tryResolve(testEnvironment.m_typeSystem);

    const babelwires::RationalType* const rationalType = type->as<babelwires::RationalType>();
    ASSERT_NE(rationalType, nullptr);

    auto range = rationalType->getRange();
    EXPECT_EQ(range.m_min, babelwires::Rational(2, 3));
    EXPECT_EQ(range.m_max, babelwires::Rational(4, 3));
}

TEST(RationalTypeTest, constructedRationalTypeIsValidValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp rationalTypeExp(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    babelwires::TypePtr type = rationalTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::RationalValue(babelwires::Rational(1, 2))));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::RationalValue(babelwires::Rational(2, 3))));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::RationalValue(babelwires::Rational(4, 3))));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::RationalValue(babelwires::Rational(3, 2))));

    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::StringValue("Hello")));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue(3)));
}

TEST(RationalTypeTest, makeTypeExp) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp rationalTypeExp = babelwires::RationalTypeConstructor::makeTypeExp(babelwires::Rational(2, 3),
        babelwires::Rational(4, 3), 1);

    babelwires::TypePtr type = rationalTypeExp.tryResolve(testEnvironment.m_typeSystem);

    const babelwires::RationalType* const rationalType = type->as<babelwires::RationalType>();
    ASSERT_NE(rationalType, nullptr);

    auto range = rationalType->getRange();
    EXPECT_EQ(range.m_min, babelwires::Rational(2, 3));
    EXPECT_EQ(range.m_max, babelwires::Rational(4, 3));

    babelwires::ValueHolder newValue = type->createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newRationalValue = newValue->as<babelwires::RationalValue>();
    EXPECT_NE(newRationalValue, nullptr);
    EXPECT_EQ(newRationalValue->get(), 1);
}

TEST(RationalTypeTest, sameKind) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp rationalTypeExp(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    babelwires::TypePtr type = rationalTypeExp.tryResolve(testEnvironment.m_typeSystem);

    babelwires::DefaultRationalType defaultRationalType;

    EXPECT_EQ(defaultRationalType.getFlavour(), type->getFlavour());
}

TEST(RationalTypeTest, typeName) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp rationalTypeExp(
        babelwires::RationalTypeConstructor::getThisIdentifier(), babelwires::RationalValue(babelwires::Rational(2, 3)),
        babelwires::RationalValue(babelwires::Rational(4, 3)), babelwires::RationalValue(1));

    EXPECT_EQ(rationalTypeExp.toString(), "Rational{2/3..1 1/3}");
}
