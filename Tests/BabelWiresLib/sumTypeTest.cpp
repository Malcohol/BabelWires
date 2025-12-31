#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>
#include <BabelWiresLib/Types/Sum/sumTypeConstructor.hpp>

#include <Domains/TestDomain/testSumType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(SumTypeTest, sumTypeDefault0) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestSumType sumType(0);

    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(sumType.getSummands()[1], babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 0);

    babelwires::ValueHolder newValue = sumType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *newValue), 0);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    EXPECT_NE(newValue->as<babelwires::IntValue>(), nullptr);

    babelwires::ValueHolder rationalValue =
        testEnvironment.m_typeSystem.getEntryByType<babelwires::DefaultRationalType>()->createValue(
            testEnvironment.m_typeSystem);

    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *rationalValue), 1);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *rationalValue));
}

TEST(SumTypeTest, sumTypeDefault1) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestSumType sumType(1);

    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(sumType.getSummands()[1], babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 1);

    babelwires::ValueHolder newValue = sumType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *newValue), 1);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    EXPECT_NE(newValue->as<babelwires::RationalValue>(), nullptr);

    babelwires::ValueHolder intValue =
        testEnvironment.m_typeSystem.getEntryByType<babelwires::DefaultIntType>()->createValue(
            testEnvironment.m_typeSystem);

    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *intValue), 0);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *intValue));
}

TEST(SumTypeTest, sumTypeConstructorNoDefaultIndex) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef sumTypeRef(babelwires::SumTypeConstructor::getThisIdentifier(),
                                   babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType());

    const babelwires::TypePtr& type = sumTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->as<babelwires::SumType>(), nullptr);

    const babelwires::SumType& sumType = type->is<babelwires::SumType>();
    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(sumType.getSummands()[1], babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 0);
}

TEST(SumTypeTest, sumTypeConstructorDefault1) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef sumTypeRef(
        babelwires::SumTypeConstructor::getThisIdentifier(),
        {{babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()},
         {babelwires::IntValue(1)}});

    const babelwires::TypePtr& type = sumTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->as<babelwires::SumType>(), nullptr);

    const babelwires::SumType& sumType = type->is<babelwires::SumType>();
    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(sumType.getSummands()[1], babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 1);
}

TEST(SumTypeTest, sumTypeConstructorMalformed) {
    testUtils::TestEnvironment testEnvironment;

    // Just one summand
    EXPECT_THROW(babelwires::TypeRef(babelwires::SumTypeConstructor::getThisIdentifier(),
                                     babelwires::DefaultIntType::getThisType())
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // Wrong value argument
    EXPECT_THROW(babelwires::TypeRef(
                     babelwires::SumTypeConstructor::getThisIdentifier(),
                     {{babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()},
                      {babelwires::RationalValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // Too many value arguments
    EXPECT_THROW(babelwires::TypeRef(
                     babelwires::SumTypeConstructor::getThisIdentifier(),
                     {{babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()},
                      {babelwires::IntValue(1), babelwires::IntValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // value argument out of range
    EXPECT_THROW(babelwires::TypeRef(
                     babelwires::SumTypeConstructor::getThisIdentifier(),
                     {{babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()},
                      {babelwires::IntValue(2)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);
}

TEST(SumTypeTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeRef sumType = testDomain::TestSumType::getThisType();

    const babelwires::TypeRef intType = babelwires::DefaultIntType::getThisType();
    const babelwires::TypeRef ratType = babelwires::DefaultRationalType::getThisType();
    const babelwires::TypeRef stringType = babelwires::StringType::getThisType();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(intType, sumType), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumType, intType), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ratType, sumType), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumType, ratType), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(stringType, sumType), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumType, stringType), babelwires::SubtypeOrder::IsDisjoint);
}

namespace {
    const std::array<babelwires::SubtypeOrder, 5> subtypeOrderElements = {
        babelwires::SubtypeOrder::IsEquivalent, babelwires::SubtypeOrder::IsSubtype,
        babelwires::SubtypeOrder::IsSupertype, babelwires::SubtypeOrder::IsIntersecting,
        babelwires::SubtypeOrder::IsDisjoint};
}

TEST(SumTypeTest, opUnionRightSymmetric) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            const babelwires::SubtypeOrder ab = babelwires::SumType::opUnionRight(a, b);
            const babelwires::SubtypeOrder ba = babelwires::SumType::opUnionRight(b, a);
            EXPECT_EQ(ab, ba);
        }
    }
}

TEST(SumTypeTest, opUnionRightAssociative) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            for (auto c : subtypeOrderElements) {
                const babelwires::SubtypeOrder ab_c =
                    babelwires::SumType::opUnionRight(babelwires::SumType::opUnionRight(a, b), c);
                const babelwires::SubtypeOrder a_bc =
                    babelwires::SumType::opUnionRight(a, babelwires::SumType::opUnionRight(c, b));
                EXPECT_EQ(ab_c, a_bc);
            }
        }
    }
}

TEST(SumTypeTest, opUnionLeftSymmetric) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            const babelwires::SubtypeOrder ab = babelwires::SumType::opUnionLeft(a, b);
            const babelwires::SubtypeOrder ba = babelwires::SumType::opUnionLeft(b, a);
            EXPECT_EQ(ab, ba);
        }
    }
}

TEST(SumTypeTest, opUnionLeftAssociative) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            for (auto c : subtypeOrderElements) {
                const babelwires::SubtypeOrder ab_c =
                    babelwires::SumType::opUnionLeft(babelwires::SumType::opUnionLeft(a, b), c);
                const babelwires::SubtypeOrder a_bc =
                    babelwires::SumType::opUnionLeft(a, babelwires::SumType::opUnionLeft(c, b));
                EXPECT_EQ(ab_c, a_bc);
            }
        }
    }
}

TEST(SumTypeTest, compareSubtype2) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeRef Zn = babelwires::IntTypeConstructor::makeTypeRef(0, 4);
    const babelwires::TypeRef Zw = babelwires::IntTypeConstructor::makeTypeRef(0, 16);
    const babelwires::TypeRef Qn = babelwires::RationalTypeConstructor::makeTypeRef(0, 4);
    const babelwires::TypeRef Qw = babelwires::RationalTypeConstructor::makeTypeRef(0, 16);
    const babelwires::TypeRef S = babelwires::StringType::getThisType();
    const babelwires::TypeRef Zd = babelwires::IntTypeConstructor::makeTypeRef(8, 16, 8);

    const babelwires::TypeRef ZnQn = babelwires::SumTypeConstructor::makeTypeRef({Zn, Qn});
    const babelwires::TypeRef QnZn = babelwires::SumTypeConstructor::makeTypeRef({Qn, Zn});
    const babelwires::TypeRef ZwQn = babelwires::SumTypeConstructor::makeTypeRef({Zw, Qn});
    const babelwires::TypeRef ZwQw = babelwires::SumTypeConstructor::makeTypeRef({Zw, Qw});
    const babelwires::TypeRef ZnQnS = babelwires::SumTypeConstructor::makeTypeRef({Zn, Qn, S});
    const babelwires::TypeRef ZdS = babelwires::SumTypeConstructor::makeTypeRef({Zd, S});
    const babelwires::TypeRef ZnZd = babelwires::SumTypeConstructor::makeTypeRef({Zn, Zd});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnQn, ZnQn), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnQn, QnZn), babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(Zn, ZnQn), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnQn, Zn), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnZd, Zw), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(Zw, ZnZd), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(Zw, ZnQn), babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnQn, Zw), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnQn, ZwQw), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZwQn, ZnQnS), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnQn, ZwQn), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZwQn, ZnQn), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZnQn, ZdS), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZdS, ZnQn), babelwires::SubtypeOrder::IsDisjoint);
}

TEST(SumTypeTest, subTypeAssociativity) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeRef Z = babelwires::DefaultIntType::getThisType();
    const babelwires::TypeRef Q = babelwires::DefaultRationalType::getThisType();
    const babelwires::TypeRef S = babelwires::StringType::getThisType();

    const babelwires::TypeRef ZQ_S =
        babelwires::SumTypeConstructor::makeTypeRef({babelwires::SumTypeConstructor::makeTypeRef({Z, Q}), S});
    const babelwires::TypeRef Z_QS =
        babelwires::SumTypeConstructor::makeTypeRef({Z, babelwires::SumTypeConstructor::makeTypeRef({Q, S})});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(ZQ_S, Z_QS), babelwires::SubtypeOrder::IsEquivalent);
}
