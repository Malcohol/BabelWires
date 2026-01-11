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

    testDomain::TestTupleType tupleType(testEnvironment.m_typeSystem);

    EXPECT_EQ(tupleType.getComponentTypes().size(), 2);
    EXPECT_EQ(tupleType.getComponentTypes()[0]->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(tupleType.getComponentTypes()[1]->getTypeExp(), babelwires::DefaultRationalType::getThisIdentifier());

    babelwires::ValueHolder newValue = tupleType.createValue(testEnvironment.m_typeSystem);
    EXPECT_TRUE(newValue);
    EXPECT_TRUE(tupleType.isValidValue(testEnvironment.m_typeSystem, *newValue));

    const auto& intType = testEnvironment.m_typeSystem.getRegisteredType<babelwires::DefaultIntType>();
    const auto& rationalType = testEnvironment.m_typeSystem.getRegisteredType<babelwires::DefaultRationalType>();

    const auto* newTuple = newValue->tryAs<babelwires::TupleValue>();
    ASSERT_NE(newTuple, nullptr);
    ASSERT_EQ(newTuple->getSize(), 2);
    EXPECT_TRUE(intType->isValidValue(testEnvironment.m_typeSystem, *newTuple->getValue(0)));
    EXPECT_TRUE(rationalType->isValidValue(testEnvironment.m_typeSystem, *newTuple->getValue(1)));
}

TEST(TupleTypeTest, valueToString) {
    testUtils::TestEnvironment testEnvironment;

    testDomain::TestTupleType tupleType(testEnvironment.m_typeSystem);
    babelwires::NewValueHolder newValue = tupleType.createValue(testEnvironment.m_typeSystem);
    auto* newTuple = newValue.m_nonConstReference.tryAs<babelwires::TupleValue>();
    ASSERT_NE(newTuple, nullptr);

    newTuple->setValue(0, babelwires::IntValue(34));
    newTuple->setValue(1, babelwires::RationalValue({1, 3}));
    
    std::string str = tupleType.valueToString(testEnvironment.m_typeSystem, newValue.m_valueHolder);

    EXPECT_EQ(str, "(34, 1/3)");
}

TEST(TupleTypeTest, tupleTypeConstructor) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp tupleTypeExp(babelwires::TupleTypeConstructor::getThisIdentifier(),
                                   babelwires::DefaultIntType::getThisIdentifier(),
                                   babelwires::DefaultRationalType::getThisIdentifier());

    babelwires::TypePtr type = tupleTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->tryAs<babelwires::TupleType>(), nullptr);

    const babelwires::TupleType& tupleType = type->as<babelwires::TupleType>();
    EXPECT_EQ(tupleType.getComponentTypes().size(), 2);
    EXPECT_EQ(tupleType.getComponentTypes()[0]->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(tupleType.getComponentTypes()[1]->getTypeExp(), babelwires::DefaultRationalType::getThisIdentifier());
}

TEST(TupleTypeTest, makeTypeExp) {
    testUtils::TestEnvironment testEnvironment;

    auto tupleTypeExp = babelwires::TupleTypeConstructor::makeTypeExp({babelwires::DefaultIntType::getThisIdentifier(), babelwires::DefaultRationalType::getThisIdentifier()});

    babelwires::TypePtr type = tupleTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type, nullptr);
    ASSERT_NE(type->tryAs<babelwires::TupleType>(), nullptr);

    const babelwires::TupleType& tupleType = type->as<babelwires::TupleType>();
    EXPECT_EQ(tupleType.getComponentTypes().size(), 2);
    EXPECT_EQ(tupleType.getComponentTypes()[0]->getTypeExp(), babelwires::DefaultIntType::getThisIdentifier());
    EXPECT_EQ(tupleType.getComponentTypes()[1]->getTypeExp(), babelwires::DefaultRationalType::getThisIdentifier());
}

TEST(TupleTypeTest, tupleTypeConstructorMalformed) {
    testUtils::TestEnvironment testEnvironment;

    // Unresolved argument
    EXPECT_THROW(babelwires::TypeExp(babelwires::TupleTypeConstructor::getThisIdentifier(),
                                     {{babelwires::DefaultIntType::getThisIdentifier(),
                                       babelwires::TypeExp(babelwires::MediumId("abcdef"))}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);

    // Unexpected value argument
    EXPECT_THROW(babelwires::TypeExp(babelwires::TupleTypeConstructor::getThisIdentifier(),
                                     {{babelwires::DefaultIntType::getThisIdentifier(),
                                       babelwires::DefaultRationalType::getThisIdentifier()},
                                      {babelwires::RationalValue(1)}})
                     .resolve(testEnvironment.m_typeSystem),
                 babelwires::TypeSystemException);
}

TEST(TupleTypeTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeExp AAAExp = babelwires::TupleTypeConstructor::makeTypeExp({testDomain::TestEnum::getThisIdentifier(), testDomain::TestEnum::getThisIdentifier(), testDomain::TestEnum::getThisIdentifier()});
    const babelwires::TypeExp AAExp = babelwires::TupleTypeConstructor::makeTypeExp({testDomain::TestEnum::getThisIdentifier(), testDomain::TestEnum::getThisIdentifier()});
    const babelwires::TypeExp AABExp = babelwires::TupleTypeConstructor::makeTypeExp({testDomain::TestEnum::getThisIdentifier(), testDomain::TestEnum::getThisIdentifier(), testDomain::TestSubEnum::getThisIdentifier()});
    const babelwires::TypeExp BACExp = babelwires::TupleTypeConstructor::makeTypeExp({testDomain::TestSubEnum::getThisIdentifier(), testDomain::TestEnum::getThisIdentifier(), testDomain::TestSubSubEnum1::getThisIdentifier()});
    const babelwires::TypeExp CABExp = babelwires::TupleTypeConstructor::makeTypeExp({testDomain::TestSubSubEnum1::getThisIdentifier(), testDomain::TestEnum::getThisIdentifier(), testDomain::TestSubEnum::getThisIdentifier()});
    const babelwires::TypeExp CXBExp = babelwires::TupleTypeConstructor::makeTypeExp({testDomain::TestSubSubEnum1::getThisIdentifier(), babelwires::DefaultIntType::getThisIdentifier(), testDomain::TestSubEnum::getThisIdentifier()});

    const babelwires::TypePtr AAA = AAAExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr AA = AAExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr AAB = AABExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr BAC = BACExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr CAB = CABExp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr CXB = CXBExp.resolve(testEnvironment.m_typeSystem);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*AAA, *AAA), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*AAA, *AAB), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*AAB, *AAA), babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*AA, *AAA), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*AAA, *AA), babelwires::SubtypeOrder::IsDisjoint);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*AAB, *BAC), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*BAC, *AAB), babelwires::SubtypeOrder::IsSubtype);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*BAC, *CAB), babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*CAB, *BAC), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*BAC, *CXB), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*CXB, *BAC), babelwires::SubtypeOrder::IsDisjoint);
}
