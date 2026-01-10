#include <gtest/gtest.h>

#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>
#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(TypeSystemTest, isSubTypesPrimitives) {
    testUtils::TestEnvironment testEnvironment;

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestType::getThisType(),
                                                       testUtils::TestType::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestType::getThisType(),
                                                        testDomain::TestEnum::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestEnum::getThisType(),
                                                        testUtils::TestType::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestEnum::getThisType(),
                                                       testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubEnum::getThisType(),
                                                       testDomain::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum1::getThisType(),
                                                       testDomain::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum2::getThisType(),
                                                       testDomain::TestSubSubEnum2::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubEnum::getThisType(),
                                                       testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum1::getThisType(),
                                                       testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum2::getThisType(),
                                                       testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum1::getThisType(),
                                                       testDomain::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum2::getThisType(),
                                                       testDomain::TestSubEnum::getThisType()));

    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestEnum::getThisType(),
                                                        testDomain::TestSubEnum::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestEnum::getThisType(),
                                                        testDomain::TestSubSubEnum1::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestEnum::getThisType(),
                                                        testDomain::TestSubSubEnum2::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubEnum::getThisType(),
                                                        testDomain::TestSubSubEnum1::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubEnum::getThisType(),
                                                        testDomain::TestSubSubEnum2::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum1::getThisType(),
                                                        testDomain::TestSubSubEnum2::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testDomain::TestSubSubEnum2::getThisType(),
                                                        testDomain::TestSubSubEnum1::getThisType()));
}

TEST(TypeSystemTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeExp testType4(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxx")}});
    const babelwires::TypeExp testType6(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxxxx")}});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType4), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType6), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testType4), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testDomain::TestEnum::getThisType()), babelwires::SubtypeOrder::IsDisjoint);
}

TEST(TypeSystemTest, isRelatedTypes) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr testType = testEnvironment.m_typeSystem.getEntryByType<testUtils::TestType>();
    const babelwires::TypePtr testEnum = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestEnum>();
    const babelwires::TypePtr testSubEnum = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestSubEnum>();
    const babelwires::TypePtr testSubSubEnum1 = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestSubSubEnum1>();
    const babelwires::TypePtr testSubSubEnum2 = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestSubSubEnum2>();

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testType, *testType));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(*testType, *testEnum));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(*testEnum, *testType));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testEnum, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubEnum, *testSubEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum1, *testSubSubEnum1));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum2, *testSubSubEnum2));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubEnum, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum1, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum2, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum1, *testSubEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum2, *testSubEnum));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testEnum, *testSubEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testEnum, *testSubSubEnum1));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testEnum, *testSubSubEnum2));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubEnum, *testSubSubEnum1));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubEnum, *testSubSubEnum2));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum1, *testSubSubEnum2));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(*testSubSubEnum2, *testSubSubEnum1));
}

TEST(TypeSystemTest, getTaggedTypes) {
    testUtils::TestEnvironment testEnvironment;   

    auto types = testEnvironment.m_typeSystem.getTaggedRegisteredTypes(testUtils::TestType::getTestTypeTag());

    EXPECT_EQ(types.size(), 1);
    EXPECT_EQ(types[0], testUtils::TestType::getThisType());
}
