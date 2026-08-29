#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Fixed/fixedType.hpp>
#include <BabelWiresLib/Types/Fixed/fixedTypeConstructor.hpp>
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

#include <Domains/TestDomain/testFixedType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(FixedTypeTest, testTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestFixedType type;

    babelwires::ValueHolder value = type.createValue(testEnvironment.m_typeSystem);
    ASSERT_TRUE(value);
    const auto* fixedValue = value->tryAs<babelwires::FixedValue>();
    ASSERT_NE(fixedValue, nullptr);
    EXPECT_EQ(fixedValue->get(), babelwires::Fixed(0, babelwires::FixedType::s_defaultPrecision));
}

TEST(FixedTypeTest, testTypeGetRange) {
    testDomain::TestFixedType type;

    auto range = type.getRange();

    EXPECT_EQ(range.m_min, -250);
    EXPECT_EQ(range.m_max, 425);
}

TEST(FixedTypeTest, testTypeIsValidValue) {
    testUtils::TestEnvironment testEnvironment;
    testDomain::TestFixedType type;

    EXPECT_TRUE(type.isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(-250, 2))));
    EXPECT_TRUE(type.isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(0, 2))));
    EXPECT_TRUE(type.isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(425, 2))));
    EXPECT_FALSE(type.isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(-251, 2))));
    EXPECT_FALSE(type.isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(426, 2))));
    EXPECT_FALSE(type.isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(1, 3))));
}

TEST(FixedTypeTest, testTypeGetKind) {
    testDomain::TestFixedType type;

    EXPECT_FALSE(type.getFlavour().empty());
}

TEST(FixedTypeTest, testTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(testDomain::TestFixedType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->tryAs<testDomain::TestFixedType>(), nullptr);
}

TEST(FixedTypeTest, constructedTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp fixedTypeExp(babelwires::FixedTypeConstructor::getThisIdentifier(),
                                     babelwires::TypeConstructorArguments{{}, {babelwires::IntValue(2),
                                                                               babelwires::FixedValue(babelwires::Fixed(-1, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(2, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(1, 2))}});

    babelwires::TypePtr type = fixedTypeExp.tryResolve(testEnvironment.m_typeSystem);
    ASSERT_TRUE(type);

    babelwires::ValueHolder value = type->createValue(testEnvironment.m_typeSystem);
    ASSERT_TRUE(value);

    const auto* fixedValue = value->tryAs<babelwires::FixedValue>();
    ASSERT_NE(fixedValue, nullptr);
    EXPECT_EQ(fixedValue->get(), babelwires::Fixed(1, 2));
}

TEST(FixedTypeTest, constructedTypeRange) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp fixedTypeExp(babelwires::FixedTypeConstructor::getThisIdentifier(),
                                     babelwires::TypeConstructorArguments{{}, {babelwires::IntValue(2),
                                                                               babelwires::FixedValue(babelwires::Fixed(-1, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(2, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(1, 2))}});

    babelwires::TypePtr type = fixedTypeExp.tryResolve(testEnvironment.m_typeSystem);
    const auto* fixedType = type->tryAs<babelwires::FixedType>();
    ASSERT_NE(fixedType, nullptr);

    auto range = fixedType->getRange();
    EXPECT_EQ(range.m_min, -1);
    EXPECT_EQ(range.m_max, 2);
}

TEST(FixedTypeTest, constructedTypeIsValidValue) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp fixedTypeExp(babelwires::FixedTypeConstructor::getThisIdentifier(),
                                     babelwires::TypeConstructorArguments{{}, {babelwires::IntValue(2),
                                                                               babelwires::FixedValue(babelwires::Fixed(-1, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(2, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(1, 2))}});

    babelwires::TypePtr type = fixedTypeExp.tryResolve(testEnvironment.m_typeSystem);
    ASSERT_TRUE(type);

    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(-2, 2))));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(-1, 2))));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(1, 2))));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(2, 2))));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(3, 2))));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(1, 3))));
}

TEST(FixedTypeTest, makeTypeExp) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp fixedTypeExp = babelwires::FixedTypeConstructor::makeTypeExp(2, babelwires::Fixed(-1, 2),
                                                                                     babelwires::Fixed(2, 2),
                                                                                     babelwires::Fixed(1, 2));

    babelwires::TypePtr type = fixedTypeExp.tryResolve(testEnvironment.m_typeSystem);
    const auto* fixedType = type->tryAs<babelwires::FixedType>();
    ASSERT_NE(fixedType, nullptr);

    auto range = fixedType->getRange();
    EXPECT_EQ(range.m_min, -1);
    EXPECT_EQ(range.m_max, 2);

    babelwires::ValueHolder value = type->createValue(testEnvironment.m_typeSystem);
    ASSERT_TRUE(value);

    const auto* fixedValue = value->tryAs<babelwires::FixedValue>();
    ASSERT_NE(fixedValue, nullptr);
    EXPECT_EQ(fixedValue->get(), babelwires::Fixed(1, 2));
}

TEST(FixedTypeTest, sameKind) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp fixedTypeExp(babelwires::FixedTypeConstructor::getThisIdentifier(),
                                     babelwires::TypeConstructorArguments{{}, {babelwires::IntValue(2),
                                                                               babelwires::FixedValue(babelwires::Fixed(-1, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(2, 2)),
                                                                               babelwires::FixedValue(babelwires::Fixed(1, 2))}});

    babelwires::TypePtr type = fixedTypeExp.tryResolve(testEnvironment.m_typeSystem);
    testDomain::TestFixedType testType;

    EXPECT_EQ(testType.getFlavour(), type->getFlavour());
}

TEST(FixedTypeTest, constructorRejectsPrecisionMismatch) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp typeExp(babelwires::FixedTypeConstructor::getThisIdentifier(),
                                babelwires::TypeConstructorArguments{{}, {babelwires::IntValue(2),
                                                                          babelwires::FixedValue(babelwires::Fixed(0, 2)),
                                                                          babelwires::FixedValue(babelwires::Fixed(10, 2)),
                                                                          babelwires::FixedValue(babelwires::Fixed(5, 2))}});

    EXPECT_TRUE(typeExp.tryResolve(testEnvironment.m_typeSystem));

    babelwires::TypeExp badTypeExp(babelwires::FixedTypeConstructor::getThisIdentifier(),
                                   babelwires::TypeConstructorArguments{{}, {babelwires::IntValue(2),
                                                                             babelwires::FixedValue(babelwires::Fixed(0, 1)),
                                                                             babelwires::FixedValue(babelwires::Fixed(10, 2)),
                                                                             babelwires::FixedValue(babelwires::Fixed(5, 2))}});

    EXPECT_FALSE(badTypeExp.tryResolve(testEnvironment.m_typeSystem));
}

TEST(FixedTypeTest, rangesAndValidity) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp typeExp(babelwires::FixedTypeConstructor::getThisIdentifier(),
                                babelwires::TypeConstructorArguments{{}, {babelwires::IntValue(2),
                                                                          babelwires::FixedValue(babelwires::Fixed(-1, 2)),
                                                                          babelwires::FixedValue(babelwires::Fixed(2, 2)),
                                                                          babelwires::FixedValue(babelwires::Fixed(1, 2))}});

    babelwires::TypePtr type = typeExp.tryResolve(testEnvironment.m_typeSystem);
    ASSERT_TRUE(type);

    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(-1, 2))));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(1, 2))));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(2, 2))));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(3, 2))));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::FixedValue(babelwires::Fixed(1, 3))));
}

TEST(FixedTypeTest, subtypeComparison) {
    testUtils::TestEnvironment testEnvironment;

    auto a = babelwires::FixedTypeConstructor::makeTypeExp(2, babelwires::Fixed(-10, 2), babelwires::Fixed(10, 2), babelwires::Fixed(0, 2));
    auto b = babelwires::FixedTypeConstructor::makeTypeExp(2, babelwires::Fixed(-5, 2), babelwires::Fixed(5, 2), babelwires::Fixed(0, 2));
    auto c = babelwires::FixedTypeConstructor::makeTypeExp(3, babelwires::Fixed(-10, 3), babelwires::Fixed(10, 3), babelwires::Fixed(0, 3));

    auto typeA = a.tryResolve(testEnvironment.m_typeSystem);
    auto typeB = b.tryResolve(testEnvironment.m_typeSystem);
    auto typeC = c.tryResolve(testEnvironment.m_typeSystem);
    ASSERT_TRUE(typeA && typeB && typeC);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*typeA, *typeB), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*typeB, *typeA), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*typeA, *typeC), babelwires::SubtypeOrder::IsDisjoint);
}
