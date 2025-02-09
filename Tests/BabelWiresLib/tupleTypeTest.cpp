#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>
//#include <BabelWiresLib/Types/Tuple/tupleTypeConstructor.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

namespace {
    class TestTupleType : public babelwires::TupleType {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("TestTupleType"), 1);
        TestTupleType()
            : babelwires::TupleType({babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType()}) {}
    };
} // namespace

TEST(TupleTypeTest, createValue) {
    testUtils::TestEnvironment testEnvironment;

    TestTupleType tupleType;

    EXPECT_EQ(tupleType.getParameterTypes().size(), 2);
    EXPECT_EQ(tupleType.getParameterTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(tupleType.getParameterTypes()[1], babelwires::DefaultRationalType::getThisType());

    babelwires::ValueHolder newValue = tupleType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_TRUE(tupleType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    const auto& intType = testEnvironment.m_typeSystem.getPrimitiveType(babelwires::DefaultIntType::getThisIdentifier());
    const auto& rationalType = testEnvironment.m_typeSystem.getPrimitiveType(babelwires::DefaultRationalType::getThisIdentifier());

    const auto* newTuple = newValue->as<babelwires::TupleValue>();
    ASSERT_NE(newTuple, nullptr);
    ASSERT_EQ(newTuple->getSize(), 2);
    EXPECT_TRUE(intType.isValidValue(testEnvironment.m_typeSystem, *newTuple->getValue(0)));
    EXPECT_TRUE(rationalType.isValidValue(testEnvironment.m_typeSystem, *newTuple->getValue(1)));
}

TEST(TupleTypeTest, valueToString) {
    testUtils::TestEnvironment testEnvironment;

    TestTupleType tupleType;
    babelwires::NewValueHolder newValue = tupleType.createValue(testEnvironment.m_typeSystem);
    auto* newTuple = newValue.m_nonConstReference.as<babelwires::TupleValue>();
    ASSERT_NE(newTuple, nullptr);

    newTuple->setValue(0, babelwires::IntValue(34));
    newTuple->setValue(1, babelwires::RationalValue({1, 3}));
    
    std::string str = tupleType.valueToString(testEnvironment.m_typeSystem, newValue.m_valueHolder);

    EXPECT_EQ(str, "(34, 1/3)");
}

/*
TEST(TupleTypeTest, sumTypeConstructorNoDefaultIndex) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef sumTypeRef(babelwires::SumTypeConstructor::getThisIdentifier(),
                                   babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType());

    const babelwires::Type* const type = sumTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->as<babelwires::TupleType>(), nullptr);

    const babelwires::TupleType& tupleType = type->is<babelwires::TupleType>();
    EXPECT_EQ(tupleType.getParameterTypes().size(), 2);
    EXPECT_EQ(tupleType.getParameterTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(tupleType.getParameterTypes()[1], babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(tupleType.getIndexOfDefaultSummand(), 0);
}

TEST(TupleTypeTest, sumTypeConstructorDefault1) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef sumTypeRef(
        babelwires::SumTypeConstructor::getThisIdentifier(),
        {{babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()},
         {babelwires::IntValue(1)}});

    const babelwires::Type* const type = sumTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->as<babelwires::TupleType>(), nullptr);

    const babelwires::TupleType& tupleType = type->is<babelwires::TupleType>();
    EXPECT_EQ(tupleType.getParameterTypes().size(), 2);
    EXPECT_EQ(tupleType.getParameterTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(tupleType.getParameterTypes()[1], babelwires::DefaultRationalType::getThisType());
    EXPECT_EQ(tupleType.getIndexOfDefaultSummand(), 1);
}

TEST(TupleTypeTest, sumTypeConstructorMalformed) {
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
*/