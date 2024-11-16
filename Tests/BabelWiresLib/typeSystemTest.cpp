#include <gtest/gtest.h>

#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
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
                                                        testUtils::TestEnum::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisType(),
                                                        testUtils::TestType::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisType(),
                                                       testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisType(),
                                                       testUtils::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisType(),
                                                       testUtils::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisType(),
                                                       testUtils::TestSubSubEnum2::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisType(),
                                                       testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisType(),
                                                       testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisType(),
                                                       testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisType(),
                                                       testUtils::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisType(),
                                                       testUtils::TestSubEnum::getThisType()));

    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisType(),
                                                        testUtils::TestSubEnum::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisType(),
                                                        testUtils::TestSubSubEnum1::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisType(),
                                                        testUtils::TestSubSubEnum2::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisType(),
                                                        testUtils::TestSubSubEnum1::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisType(),
                                                        testUtils::TestSubSubEnum2::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisType(),
                                                        testUtils::TestSubSubEnum2::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisType(),
                                                        testUtils::TestSubSubEnum1::getThisType()));
}

TEST(TypeSystemTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeRef testType4(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxx")}});
    const babelwires::TypeRef testType6(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxxxx")}});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType4), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType6), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testType4), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testUtils::TestEnum::getThisType()), babelwires::SubtypeOrder::IsUnrelated);
}

TEST(TypeSystemTest, isRelatedTypes) {
    testUtils::TestEnvironment testEnvironment;

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestType::getThisType(),
                                                           testUtils::TestType::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestType::getThisType(),
                                                            testUtils::TestEnum::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisType(),
                                                            testUtils::TestType::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisType(),
                                                           testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisType(),
                                                           testUtils::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisType(),
                                                           testUtils::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisType(),
                                                           testUtils::TestSubSubEnum2::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisType(),
                                                           testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisType(),
                                                           testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisType(),
                                                           testUtils::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisType(),
                                                           testUtils::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisType(),
                                                           testUtils::TestSubEnum::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisType(),
                                                           testUtils::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisType(),
                                                           testUtils::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisType(),
                                                           testUtils::TestSubSubEnum2::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisType(),
                                                           testUtils::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisType(),
                                                           testUtils::TestSubSubEnum2::getThisType()));

    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisType(),
                                                            testUtils::TestSubSubEnum2::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisType(),
                                                            testUtils::TestSubSubEnum1::getThisType()));
}

TEST(TypeSystemTest, getTaggedTypes) {
    testUtils::TestEnvironment testEnvironment;   

    auto types = testEnvironment.m_typeSystem.getTaggedPrimitiveTypes(testUtils::TestType::getTestTypeTag());

    EXPECT_EQ(types.size(), 1);
    EXPECT_EQ(types[0], testUtils::TestType::getThisType());
}
