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

    testDomain::TestSumType sumType(testEnvironment.m_typeSystem, 0);

    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0]->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(sumType.getSummands()[1]->getTypeExp(), babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 0);

    babelwires::ValueHolder newValue = sumType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *newValue), 0);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    EXPECT_NE(newValue->tryAs<babelwires::IntValue>(), nullptr);

    babelwires::ValueHolder rationalValue =
        testEnvironment.m_typeSystem.getRegisteredType<babelwires::DefaultRationalType>()->createValue(
            testEnvironment.m_typeSystem);

    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *rationalValue), 1);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *rationalValue));
}

TEST(SumTypeTest, sumTypeDefault1) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestSumType sumType(testEnvironment.m_typeSystem, 1);

    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0]->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(sumType.getSummands()[1]->getTypeExp(), babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 1);

    babelwires::ValueHolder newValue = sumType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *newValue), 1);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    EXPECT_NE(newValue->tryAs<babelwires::RationalValue>(), nullptr);

    babelwires::ValueHolder intValue =
        testEnvironment.m_typeSystem.getRegisteredType<babelwires::DefaultIntType>()->createValue(
            testEnvironment.m_typeSystem);

    EXPECT_EQ(sumType.getIndexOfValue(testEnvironment.m_typeSystem, *intValue), 0);
    EXPECT_TRUE(sumType.isValidValue(testEnvironment.m_typeSystem, *intValue));
}

TEST(SumTypeTest, sumTypeConstructorNoDefaultIndex) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp sumTypeExp(babelwires::SumTypeConstructor::getThisIdentifier(),
                                   babelwires::DefaultIntType::getThisIdentifier(),
                                   babelwires::DefaultRationalType::getThisIdentifier());

    babelwires::TypePtr type = sumTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->tryAs<babelwires::SumType>(), nullptr);

    const babelwires::SumType& sumType = type->as<babelwires::SumType>();
    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0]->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(sumType.getSummands()[1]->getTypeExp(), babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 0);
}

TEST(SumTypeTest, sumTypeConstructorDefault1) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp sumTypeExp(
        babelwires::SumTypeConstructor::getThisIdentifier(),
        {{babelwires::DefaultIntType::getThisIdentifier(), babelwires::DefaultRationalType::getThisIdentifier()},
         {babelwires::IntValue(1)}});

    babelwires::TypePtr type = sumTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->tryAs<babelwires::SumType>(), nullptr);

    const babelwires::SumType& sumType = type->as<babelwires::SumType>();
    EXPECT_EQ(sumType.getSummands().size(), 2);
    EXPECT_EQ(sumType.getSummands()[0]->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(sumType.getSummands()[1]->getTypeExp(), babelwires::DefaultRationalType::getThisIdentifier());
    EXPECT_EQ(sumType.getIndexOfDefaultSummand(), 1);
}

TEST(SumTypeTest, sumTypeConstructorMalformed) {
    testUtils::TestEnvironment testEnvironment;

    // Just one summand
    EXPECT_THROW(babelwires::TypeExp(babelwires::SumTypeConstructor::getThisIdentifier(),
                                     babelwires::DefaultIntType::getThisIdentifier())
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // Wrong value argument
    EXPECT_THROW(babelwires::TypeExp(
                     babelwires::SumTypeConstructor::getThisIdentifier(),
                     {{babelwires::DefaultIntType::getThisIdentifier(), babelwires::DefaultRationalType::getThisIdentifier()},
                      {babelwires::RationalValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // Too many value arguments
    EXPECT_THROW(babelwires::TypeExp(
                     babelwires::SumTypeConstructor::getThisIdentifier(),
                     {{babelwires::DefaultIntType::getThisIdentifier(), babelwires::DefaultRationalType::getThisIdentifier()},
                      {babelwires::IntValue(1), babelwires::IntValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // value argument out of range
    EXPECT_THROW(babelwires::TypeExp(
                     babelwires::SumTypeConstructor::getThisIdentifier(),
                     {{babelwires::DefaultIntType::getThisIdentifier(), babelwires::DefaultRationalType::getThisIdentifier()},
                      {babelwires::IntValue(2)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);
}

TEST(SumTypeTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr sumType = testEnvironment.m_typeSystem.getRegisteredType<testDomain::TestSumType>();
    const babelwires::TypePtr intType = testEnvironment.m_typeSystem.getRegisteredType<babelwires::DefaultIntType>();
    const babelwires::TypePtr ratType = testEnvironment.m_typeSystem.getRegisteredType<babelwires::DefaultRationalType>();
    const babelwires::TypePtr stringType = testEnvironment.m_typeSystem.getRegisteredType<babelwires::StringType>();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*intType, *sumType), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*sumType, *intType), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ratType, *sumType), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*sumType, *ratType), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*stringType, *sumType), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*sumType, *stringType), babelwires::SubtypeOrder::IsDisjoint);
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

    const babelwires::TypeExp ZnExp = babelwires::IntTypeConstructor::makeTypeExp(0, 4);
    const babelwires::TypeExp ZwExp = babelwires::IntTypeConstructor::makeTypeExp(0, 16);
    const babelwires::TypeExp QnExp = babelwires::RationalTypeConstructor::makeTypeExp(0, 4);
    const babelwires::TypeExp QwExp = babelwires::RationalTypeConstructor::makeTypeExp(0, 16);
    const babelwires::TypeExp SExp = babelwires::StringType::getThisIdentifier();
    const babelwires::TypeExp ZdExp = babelwires::IntTypeConstructor::makeTypeExp(8, 16, 8);

    const babelwires::TypeExp ZnQnExp = babelwires::SumTypeConstructor::makeTypeExp({ZnExp, QnExp});
    const babelwires::TypeExp QnZnExp = babelwires::SumTypeConstructor::makeTypeExp({QnExp, ZnExp});
    const babelwires::TypeExp ZwQnExp = babelwires::SumTypeConstructor::makeTypeExp({ZwExp, QnExp});
    const babelwires::TypeExp ZwQwExp = babelwires::SumTypeConstructor::makeTypeExp({ZwExp, QwExp});
    const babelwires::TypeExp ZnQnSExp = babelwires::SumTypeConstructor::makeTypeExp({ZnExp, QnExp, SExp});
    const babelwires::TypeExp ZdSExp = babelwires::SumTypeConstructor::makeTypeExp({ZdExp, SExp});
    const babelwires::TypeExp ZnZdExp = babelwires::SumTypeConstructor::makeTypeExp({ZnExp, ZdExp});

    const babelwires::TypePtr Zn = ZnExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr Zw = ZwExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr ZnQn = ZnQnExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr QnZn = QnZnExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr ZwQn = ZwQnExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr ZwQw = ZwQwExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr ZnQnS = ZnQnSExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr ZdS = ZdSExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr ZnZd = ZnZdExp.resolve(testEnvironment.m_typeSystem);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnQn, *ZnQn), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnQn, *QnZn), babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*Zn, *ZnQn), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnQn, *Zn), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnZd, *Zw), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*Zw, *ZnZd), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*Zw, *ZnQn), babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnQn, *Zw), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnQn, *ZwQw), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZwQn, *ZnQnS), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnQn, *ZwQn), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZwQn, *ZnQn), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZnQn, *ZdS), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZdS, *ZnQn), babelwires::SubtypeOrder::IsDisjoint);
}

TEST(SumTypeTest, subTypeAssociativity) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeExp Z = babelwires::DefaultIntType::getThisIdentifier();
    const babelwires::TypeExp Q = babelwires::DefaultRationalType::getThisIdentifier();
    const babelwires::TypeExp S = babelwires::StringType::getThisIdentifier();

    const babelwires::TypeExp ZQ_S =
        babelwires::SumTypeConstructor::makeTypeExp({babelwires::SumTypeConstructor::makeTypeExp({Z, Q}), S});
    const babelwires::TypeExp Z_QS =
        babelwires::SumTypeConstructor::makeTypeExp({Z, babelwires::SumTypeConstructor::makeTypeExp({Q, S})});

    const babelwires::TypePtr ZQ_S_ptr = ZQ_S.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr Z_QS_ptr = Z_QS.resolve(testEnvironment.m_typeSystem);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*ZQ_S_ptr, *Z_QS_ptr), babelwires::SubtypeOrder::IsEquivalent);
}
