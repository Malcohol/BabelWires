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

    const babelwires::TypeRef testType4(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxx")}});
    const babelwires::TypeRef testType6(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisType()}, {babelwires::StringValue("xxxxxx")}});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType4), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType6), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testType4), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testDomain::TestEnum::getThisType()), babelwires::SubtypeOrder::IsDisjoint);
}

TEST(TypeSystemTest, isRelatedTypes) {
    testUtils::TestEnvironment testEnvironment;

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestType::getThisType(),
                                                           testUtils::TestType::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestType::getThisType(),
                                                            testDomain::TestEnum::getThisType()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestEnum::getThisType(),
                                                            testUtils::TestType::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestEnum::getThisType(),
                                                           testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubEnum::getThisType(),
                                                           testDomain::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum1::getThisType(),
                                                           testDomain::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum2::getThisType(),
                                                           testDomain::TestSubSubEnum2::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubEnum::getThisType(),
                                                           testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum1::getThisType(),
                                                           testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum2::getThisType(),
                                                           testDomain::TestEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum1::getThisType(),
                                                           testDomain::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum2::getThisType(),
                                                           testDomain::TestSubEnum::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestEnum::getThisType(),
                                                           testDomain::TestSubEnum::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestEnum::getThisType(),
                                                           testDomain::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestEnum::getThisType(),
                                                           testDomain::TestSubSubEnum2::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubEnum::getThisType(),
                                                           testDomain::TestSubSubEnum1::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubEnum::getThisType(),
                                                           testDomain::TestSubSubEnum2::getThisType()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum1::getThisType(),
                                                            testDomain::TestSubSubEnum2::getThisType()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testDomain::TestSubSubEnum2::getThisType(),
                                                            testDomain::TestSubSubEnum1::getThisType()));
}

TEST(TypeSystemTest, getTaggedTypes) {
    testUtils::TestEnvironment testEnvironment;   

    auto types = testEnvironment.m_typeSystem.getTaggedPrimitiveTypes(testUtils::TestType::getTestTypeTag());

    EXPECT_EQ(types.size(), 1);
    EXPECT_EQ(types[0], testUtils::TestType::getThisType());
}

TEST(TypeSystemTest, subtypeFromRanges) {
    const babelwires::Range<int> range04(0, 4);
    const babelwires::Range<int> range03(0, 3);
    const babelwires::Range<int> range13(1, 3);
    const babelwires::Range<int> range14(1, 4);
    const babelwires::Range<int> range48(4, 8);
    const babelwires::Range<int> range59(5, 9);
    
    EXPECT_EQ(babelwires::subtypeFromRanges(range13, range13), babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(babelwires::subtypeFromRanges(range13, range04), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range13), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(babelwires::subtypeFromRanges(range03, range04), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range03), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(babelwires::subtypeFromRanges(range13, range14), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(babelwires::subtypeFromRanges(range14, range13), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range48), babelwires::SubtypeOrder::IsIntersecting);
    EXPECT_EQ(babelwires::subtypeFromRanges(range48, range04), babelwires::SubtypeOrder::IsIntersecting);

    EXPECT_EQ(babelwires::subtypeFromRanges(range04, range59), babelwires::SubtypeOrder::IsDisjoint);
    EXPECT_EQ(babelwires::subtypeFromRanges(range59, range04), babelwires::SubtypeOrder::IsDisjoint);
}