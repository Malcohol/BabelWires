#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(IntTypeTest, defaultIntTypeCreateValue) {
    babelwires::DefaultIntType intType;

    babelwires::ValueHolder newValue = intType.createValue();
    EXPECT_TRUE(newValue);

    const auto* const newIntValue = newValue->as<babelwires::IntValue>();
    EXPECT_NE(newIntValue, nullptr);
    EXPECT_EQ(newIntValue->get(), 0);
}

TEST(IntTypeTest, defaultIntTypeGetRange) {
    babelwires::DefaultIntType intType;

    auto range = intType.getRange();

    EXPECT_EQ(range.m_min, std::numeric_limits<babelwires::IntValue::NativeType>::min());
    EXPECT_EQ(range.m_max, std::numeric_limits<babelwires::IntValue::NativeType>::max());
}

TEST(IntTypeTest, defaultIntTypeIsValidValue) {
    babelwires::DefaultIntType intType;

    babelwires::IntValue value(80);

    EXPECT_TRUE(intType.isValidValue(value));

    const babelwires::IntValue minValue = std::numeric_limits<babelwires::IntValue::NativeType>::min();
    const babelwires::IntValue maxValue = std::numeric_limits<babelwires::IntValue::NativeType>::max();

    EXPECT_TRUE(intType.isValidValue(minValue));
    EXPECT_TRUE(intType.isValidValue(maxValue));

    EXPECT_FALSE(intType.isValidValue(babelwires::StringValue("Hello")));
    EXPECT_FALSE(intType.isValidValue(babelwires::RationalValue(3)));
}

TEST(IntTypeTest, defaultIntTypeGetKind) {
    babelwires::DefaultIntType intType;

    EXPECT_FALSE(intType.getKind().empty());
}

TEST(IntTypeTest, defaultIntTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const auto* const foundType =
        testEnvironment.m_typeSystem.tryGetPrimitiveType(babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_NE(foundType, nullptr);
    EXPECT_NE(foundType->as<babelwires::DefaultIntType>(), nullptr);
}

TEST(IntTypeTest, constructedIntTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef intTypeRef(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    const babelwires::Type* const intType = intTypeRef.tryResolve(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = intType->createValue();
    EXPECT_TRUE(newValue);

    const auto* const newIntValue = newValue->as<babelwires::IntValue>();
    EXPECT_NE(newIntValue, nullptr);
    EXPECT_EQ(newIntValue->get(), -3);
}

TEST(IntTypeTest, constructedIntTypeRange) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef intTypeRef(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    const babelwires::Type* const type = intTypeRef.tryResolve(testEnvironment.m_typeSystem);

    const babelwires::IntType* const intType = type->as<babelwires::IntType>();
    ASSERT_NE(intType, nullptr);

    auto range = intType->getRange();
    EXPECT_EQ(range.m_min, -12);
    EXPECT_EQ(range.m_max, 14);
}

TEST(IntTypeTest, constructedIntTypeIsValidValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef intTypeRef(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    const babelwires::Type* const type = intTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_FALSE(type->isValidValue(babelwires::IntValue(-13)));
    EXPECT_TRUE(type->isValidValue(babelwires::IntValue(-12)));
    EXPECT_TRUE(type->isValidValue(babelwires::IntValue(14)));
    EXPECT_FALSE(type->isValidValue(babelwires::IntValue(15)));

    EXPECT_FALSE(type->isValidValue(babelwires::StringValue("Hello")));
    EXPECT_FALSE(type->isValidValue(babelwires::RationalValue(3)));
}

TEST(IntTypeTest, sameKind) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef intTypeRef(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    const babelwires::Type* const type = intTypeRef.tryResolve(testEnvironment.m_typeSystem);

    babelwires::DefaultIntType defaultIntType;

    EXPECT_EQ(defaultIntType.getKind(), type->getKind());
}