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

    const babelwires::TypePtr testType = testEnvironment.m_typeSystem.getEntryByType<testUtils::TestType>();
    const babelwires::TypePtr testEnum = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestEnum>();
    const babelwires::TypePtr testSubEnum = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestSubEnum>();
    const babelwires::TypePtr testSubSubEnum1 = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestSubSubEnum1>();
    const babelwires::TypePtr testSubSubEnum2 = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestSubSubEnum2>();

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testType, *testType));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testType, *testEnum));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testEnum, *testType));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testEnum, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubEnum, *testSubEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum1, *testSubSubEnum1));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum2, *testSubSubEnum2));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubEnum, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum1, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum2, *testEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum1, *testSubEnum));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum2, *testSubEnum));

    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testEnum, *testSubEnum));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testEnum, *testSubSubEnum1));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testEnum, *testSubSubEnum2));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testSubEnum, *testSubSubEnum1));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testSubEnum, *testSubSubEnum2));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum1, *testSubSubEnum2));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(*testSubSubEnum2, *testSubSubEnum1));
}

TEST(TypeSystemTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeExp testType4Exp(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxx")}});
    const babelwires::TypeExp testType6Exp(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxxxx")}});
    
    const babelwires::TypePtr testType4 = testType4Exp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr testType6 = testType6Exp.resolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr testEnum = testEnvironment.m_typeSystem.getEntryByType<testDomain::TestEnum>();

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*testType4, *testType4), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*testType4, *testType6), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*testType6, *testType4), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*testType6, *testEnum), babelwires::SubtypeOrder::IsDisjoint);
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
