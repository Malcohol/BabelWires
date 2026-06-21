#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(IntTypeTest, defaultIntTypeCreateValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultIntType intType;

    babelwires::ValueHolder newValue = intType.createValue(typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newIntValue = newValue->tryAs<babelwires::IntValue>();
    EXPECT_NE(newIntValue, nullptr);
    EXPECT_EQ(newIntValue->get(), 0);
}

TEST(IntTypeTest, defaultIntTypeGetRange) {
    testUtils::TestLog log;
    babelwires::DefaultIntType intType;

    auto range = intType.getRange();

    EXPECT_EQ(range.m_min, std::numeric_limits<babelwires::IntValue::NativeType>::min());
    EXPECT_EQ(range.m_max, std::numeric_limits<babelwires::IntValue::NativeType>::max());
}

TEST(IntTypeTest, defaultIntTypeIsValidValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultIntType intType;

    babelwires::IntValue value(80);

    EXPECT_TRUE(intType.isValidValue(typeSystem, value));

    const babelwires::IntValue minValue = std::numeric_limits<babelwires::IntValue::NativeType>::min();
    const babelwires::IntValue maxValue = std::numeric_limits<babelwires::IntValue::NativeType>::max();

    EXPECT_TRUE(intType.isValidValue(typeSystem, minValue));
    EXPECT_TRUE(intType.isValidValue(typeSystem, maxValue));

    EXPECT_FALSE(intType.isValidValue(typeSystem, babelwires::TextValue(u8"Hello")));
    EXPECT_FALSE(intType.isValidValue(typeSystem, babelwires::RationalValue(3)));
}

TEST(IntTypeTest, defaultIntTypeGetKind) {
    testUtils::TestLog log;
    babelwires::DefaultIntType intType;

    EXPECT_FALSE(intType.getFlavour().empty());
}

TEST(IntTypeTest, defaultIntTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->tryAs<babelwires::DefaultIntType>(), nullptr);
}

TEST(IntTypeTest, constructedIntTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp intTypeExp(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    babelwires::TypePtr intType = intTypeExp.tryResolve(testEnvironment.m_typeSystem);

    babelwires::ValueHolder newValue = intType->createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newIntValue = newValue->tryAs<babelwires::IntValue>();
    EXPECT_NE(newIntValue, nullptr);
    EXPECT_EQ(newIntValue->get(), -3);
}

TEST(IntTypeTest, constructedIntTypeRange) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp intTypeExp(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    babelwires::TypePtr type = intTypeExp.tryResolve(testEnvironment.m_typeSystem);

    const babelwires::IntType* const intType = type->tryAs<babelwires::IntType>();
    ASSERT_NE(intType, nullptr);

    auto range = intType->getRange();
    EXPECT_EQ(range.m_min, -12);
    EXPECT_EQ(range.m_max, 14);
}

TEST(IntTypeTest, constructedIntTypeIsValidValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp intTypeExp(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    babelwires::TypePtr type = intTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue(-13)));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue(-12)));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue(14)));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue(15)));

    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"Hello")));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::RationalValue(3)));
}

TEST(IntTypeTest, makeTypeExp) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp intTypeExp = babelwires::IntTypeConstructor::makeTypeExp(-120, 140, -30);

    babelwires::TypePtr type = intTypeExp.tryResolve(testEnvironment.m_typeSystem);

    const babelwires::IntType* const intType = type->tryAs<babelwires::IntType>();
    ASSERT_NE(intType, nullptr);

    auto range = intType->getRange();
    EXPECT_EQ(range.m_min, -120);
    EXPECT_EQ(range.m_max, 140);

    babelwires::ValueHolder newValue = intType->createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newIntValue = newValue->tryAs<babelwires::IntValue>();
    EXPECT_NE(newIntValue, nullptr);
    EXPECT_EQ(newIntValue->get(), -30);
}

TEST(IntTypeTest, sameKind) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp intTypeExp(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    babelwires::TypePtr type = intTypeExp.tryResolve(testEnvironment.m_typeSystem);

    babelwires::DefaultIntType defaultIntType;

    EXPECT_EQ(defaultIntType.getFlavour(), type->getFlavour());
}

TEST(IntTypeTest, typeName) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp intTypeExp(babelwires::IntTypeConstructor::getThisIdentifier(), babelwires::IntValue(-12),
                                   babelwires::IntValue(14), babelwires::IntValue(-3));

    EXPECT_EQ(intTypeExp.toString(), "Integer{-12..14}");
}

TEST(IntTypeTest, subtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr intTypeA =
        babelwires::IntTypeConstructor::makeTypeExp(-5, 5, 0).assertResolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr intTypeB =
        babelwires::IntTypeConstructor::makeTypeExp(-10, 10, 0).assertResolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr intTypeC =
        babelwires::IntTypeConstructor::makeTypeExp(0, 10, 0).assertResolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr intTypeD =
        babelwires::IntTypeConstructor::makeTypeExp(20, 30, 20).assertResolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr defaultIntType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::DefaultIntType::getThisIdentifier());

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeA, *intTypeA), babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeA, *intTypeB), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeB, *intTypeA), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeA, *intTypeC), babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeC, *intTypeA), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeA, *intTypeD), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeD, *intTypeA), babelwires::SubtypeOrder::IsDisjoint);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intTypeA, *defaultIntType), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*defaultIntType, *intTypeA), babelwires::SubtypeOrder::IsSupertype);
}

