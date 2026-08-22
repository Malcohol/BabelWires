#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Fixed/fixedType.hpp>
#include <BabelWiresLib/Types/Fixed/fixedTypeConstructor.hpp>
#include <BabelWiresLib/Types/Fixed/fixedValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(FixedTypeTest, defaultTypeCreateValue) {
    testUtils::TestEnvironment testEnvironment;
    babelwires::DefaultFixedType type;

    babelwires::ValueHolder value = type.createValue(testEnvironment.m_typeSystem);
    ASSERT_TRUE(value);
    const auto* fixedValue = value->tryAs<babelwires::FixedValue>();
    ASSERT_NE(fixedValue, nullptr);
    EXPECT_EQ(fixedValue->get(), babelwires::Fixed(0, babelwires::FixedType::s_defaultPrecision));
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
