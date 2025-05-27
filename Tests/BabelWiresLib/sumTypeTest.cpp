#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
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
        testEnvironment.m_typeSystem.getPrimitiveType(babelwires::DefaultRationalType::getThisIdentifier())
            .createValue(testEnvironment.m_typeSystem);

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
        testEnvironment.m_typeSystem.getPrimitiveType(babelwires::DefaultIntType::getThisIdentifier())
            .createValue(testEnvironment.m_typeSystem);

    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *intValue), 0);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *intValue));
}

TEST(SumTypeTest, sumTypeConstructorNoDefaultIndex) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef sumTypeRef(babelwires::SumTypeConstructor::getThisIdentifier(),
                                   babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType());

    const babelwires::Type* const type = sumTypeRef.tryResolve(testEnvironment.m_typeSystem);

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

    const babelwires::Type* const type = sumTypeRef.tryResolve(testEnvironment.m_typeSystem);

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

TEST(SumTypeTest, opInnerSymmetric) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            const babelwires::SubtypeOrder ab = babelwires::SumType::opInner(a, b);
            const babelwires::SubtypeOrder ba = babelwires::SumType::opInner(b, a);
            EXPECT_EQ(ab, ba);
        }
    }
}

TEST(SumTypeTest, opInnerAssociative) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            for (auto c : subtypeOrderElements) {
                const babelwires::SubtypeOrder ab_c =
                    babelwires::SumType::opInner(babelwires::SumType::opInner(a, b), c);
                const babelwires::SubtypeOrder a_bc =
                    babelwires::SumType::opInner(a, babelwires::SumType::opInner(c, b));
                EXPECT_EQ(ab_c, a_bc);
            }
        }
    }
}

TEST(SumTypeTest, opOuterSymmetric) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            const babelwires::SubtypeOrder ab = babelwires::SumType::opOuter(a, b);
            const babelwires::SubtypeOrder ba = babelwires::SumType::opOuter(b, a);
            EXPECT_EQ(ab, ba);
        }
    }
}

TEST(SumTypeTest, opOuterAssociative) {
    for (auto a : subtypeOrderElements) {
        for (auto b : subtypeOrderElements) {
            for (auto c : subtypeOrderElements) {
                const babelwires::SubtypeOrder ab_c =
                    babelwires::SumType::opOuter(babelwires::SumType::opOuter(a, b), c);
                const babelwires::SubtypeOrder a_bc =
                    babelwires::SumType::opOuter(a, babelwires::SumType::opOuter(c, b));
                EXPECT_EQ(ab_c, a_bc);
            }
        }
    }
}

TEST(SumTypeTest, compareSubtype2) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeRef sumTypeNN = testDomain::TestSumTypeZnQn::getThisType();
    const babelwires::TypeRef sumTypeWN = testDomain::TestSumTypeZwQn::getThisType();
    const babelwires::TypeRef sumTypeWW = testDomain::TestSumTypeZwQw::getThisType();
    const babelwires::TypeRef sumTypeNWS = testDomain::TestSumTypeZnQwS::getThisType();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumTypeNN, sumTypeNN), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumTypeNN, sumTypeWW), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumTypeWN, sumTypeNWS), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumTypeNN, sumTypeWN), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(sumTypeWN, sumTypeNN), babelwires::SubtypeOrder::IsSupertype);
}