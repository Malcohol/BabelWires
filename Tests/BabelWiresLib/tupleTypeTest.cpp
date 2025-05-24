#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>
#include <BabelWiresLib/Types/Tuple/tupleTypeConstructor.hpp>

#include <Domains/TestDomain/testEnum.hpp>
#include <Domains/TestDomain/testTupleType.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(TupleTypeTest, createValue) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestTupleType tupleType;

    EXPECT_EQ(tupleType.getComponentTypes().size(), 2);
    EXPECT_EQ(tupleType.getComponentTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(tupleType.getComponentTypes()[1], babelwires::DefaultRationalType::getThisType());

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

    testDomain::TestTupleType tupleType;
    babelwires::NewValueHolder newValue = tupleType.createValue(testEnvironment.m_typeSystem);
    auto* newTuple = newValue.m_nonConstReference.as<babelwires::TupleValue>();
    ASSERT_NE(newTuple, nullptr);

    newTuple->setValue(0, babelwires::IntValue(34));
    newTuple->setValue(1, babelwires::RationalValue({1, 3}));
    
    std::string str = tupleType.valueToString(testEnvironment.m_typeSystem, newValue.m_valueHolder);

    EXPECT_EQ(str, "(34, 1/3)");
}

TEST(TupleTypeTest, tupleTypeConstructor) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef tupleTypeRef(babelwires::TupleTypeConstructor::getThisIdentifier(),
                                   babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType());

    const babelwires::Type* const type = tupleTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->as<babelwires::TupleType>(), nullptr);

    const babelwires::TupleType& tupleType = type->is<babelwires::TupleType>();
    EXPECT_EQ(tupleType.getComponentTypes().size(), 2);
    EXPECT_EQ(tupleType.getComponentTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(tupleType.getComponentTypes()[1], babelwires::DefaultRationalType::getThisType());
}

TEST(TupleTypeTest, makeTypeRef) {
    testUtils::TestEnvironment testEnvironment;

    auto tupleTypeRef = babelwires::TupleTypeConstructor::makeTypeRef({babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()});

    const babelwires::Type* const type = tupleTypeRef.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->as<babelwires::TupleType>(), nullptr);

    const babelwires::TupleType& tupleType = type->is<babelwires::TupleType>();
    EXPECT_EQ(tupleType.getComponentTypes().size(), 2);
    EXPECT_EQ(tupleType.getComponentTypes()[0], babelwires::DefaultIntType::getThisType());
    EXPECT_EQ(tupleType.getComponentTypes()[1], babelwires::DefaultRationalType::getThisType());
}

TEST(TupleTypeTest, tupleTypeConstructorMalformed) {
    testUtils::TestEnvironment testEnvironment;

    // Unresolved argument
    EXPECT_THROW(babelwires::TypeRef(babelwires::TupleTypeConstructor::getThisIdentifier(),
                                     {{babelwires::DefaultIntType::getThisType(),
                                       babelwires::TypeRef(babelwires::MediumId("abcdef"))}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // Unexpected value argument
    EXPECT_THROW(babelwires::TypeRef(babelwires::TupleTypeConstructor::getThisIdentifier(),
                                     {{babelwires::DefaultIntType::getThisType(),
                                       babelwires::DefaultRationalType::getThisType()},
                                      {babelwires::RationalValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);
}

TEST(TupleTypeTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeRef AAA = babelwires::TupleTypeConstructor::makeTypeRef({testDomain::TestEnum::getThisType(), testDomain::TestEnum::getThisType(), testDomain::TestEnum::getThisType()});
    const babelwires::TypeRef AA = babelwires::TupleTypeConstructor::makeTypeRef({testDomain::TestEnum::getThisType(), testDomain::TestEnum::getThisType()});

    const babelwires::TypeRef AAB = babelwires::TupleTypeConstructor::makeTypeRef({testDomain::TestEnum::getThisType(), testDomain::TestEnum::getThisType(), testDomain::TestSubEnum::getThisType()});
    const babelwires::TypeRef BAC = babelwires::TupleTypeConstructor::makeTypeRef({testDomain::TestSubEnum::getThisType(), testDomain::TestEnum::getThisType(), testDomain::TestSubSubEnum1::getThisType()});
    const babelwires::TypeRef CAB = babelwires::TupleTypeConstructor::makeTypeRef({testDomain::TestSubSubEnum1::getThisType(), testDomain::TestEnum::getThisType(), testDomain::TestSubEnum::getThisType()});
    const babelwires::TypeRef CXB = babelwires::TupleTypeConstructor::makeTypeRef({testDomain::TestSubSubEnum1::getThisType(), babelwires::DefaultIntType::getThisType(), testDomain::TestSubEnum::getThisType()});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(AAA, AAA), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(AAA, AAB), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(AAB, AAA), babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(AA, AAA), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(AAA, AA), babelwires::SubtypeOrder::IsDisjoint);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(AAB, BAC), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(BAC, AAB), babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(BAC, CAB), babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(CAB, BAC), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(BAC, CXB), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(CXB, BAC), babelwires::SubtypeOrder::IsDisjoint);
}
