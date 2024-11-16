#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>
#include <BabelWiresLib/Types/Sum/sumTypeConstructor.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    class TestSumType : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("TestSumType"), 1);
        TestSumType(unsigned int defaultType)
            : babelwires::SumType({babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType()},
                                  defaultType) {}
    };
} // namespace

TEST(SumTypeTest, sumTypeDefault0) {
    testUtils::TestEnvironment testEnvironment;

    TestSumType sumType(0);

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

    TestSumType sumType(1);

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
    EXPECT_THROW(babelwires::TypeRef(babelwires::SumTypeConstructor::getThisIdentifier(),
                                     {{babelwires::DefaultIntType::getThisType(),
                                       babelwires::DefaultRationalType::getThisType()},
                                      {babelwires::RationalValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // Too many value arguments
    EXPECT_THROW(babelwires::TypeRef(babelwires::SumTypeConstructor::getThisIdentifier(),
                                     {{babelwires::DefaultIntType::getThisType(),
                                       babelwires::DefaultRationalType::getThisType()},
                                      {babelwires::IntValue(1), babelwires::IntValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // value argument out of range
    EXPECT_THROW(babelwires::TypeRef(babelwires::SumTypeConstructor::getThisIdentifier(),
                                     {{babelwires::DefaultIntType::getThisType(),
                                       babelwires::DefaultRationalType::getThisType()},
                                      {babelwires::IntValue(2)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);
}