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

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestType::getThisIdentifier(),
                                                       testUtils::TestType::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestType::getThisIdentifier(),
                                                        testUtils::TestEnum::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(),
                                                        testUtils::TestType::getThisIdentifier()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(),
                                                       testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(),
                                                       testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                       testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                       testUtils::TestSubSubEnum2::getThisIdentifier()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(),
                                                       testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                       testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                       testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                       testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                       testUtils::TestSubEnum::getThisIdentifier()));

    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(),
                                                        testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(),
                                                        testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(),
                                                        testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(),
                                                        testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(),
                                                        testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                        testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                        testUtils::TestSubSubEnum1::getThisIdentifier()));
}

TEST(TypeSystemTest, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypeRef testType4(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisIdentifier()}, {babelwires::StringValue("xxxx")}});
    const babelwires::TypeRef testType6(testUtils::TestMixedTypeConstructor::getThisIdentifier(), babelwires::TypeConstructorArguments{{testUtils::TestType::getThisIdentifier()}, {babelwires::StringValue("xxxxxx")}});

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType4), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType4, testType6), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testType4), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testType6, testUtils::TestEnum::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);
}

TEST(TypeSystemTest, isRelatedTypes) {
    testUtils::TestEnvironment testEnvironment;

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestType::getThisIdentifier(),
                                                           testUtils::TestType::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestType::getThisIdentifier(),
                                                            testUtils::TestEnum::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                                            testUtils::TestType::getThisIdentifier()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                                           testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                                           testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                           testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                           testUtils::TestSubSubEnum2::getThisIdentifier()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                                           testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                           testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                           testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                           testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                           testUtils::TestSubEnum::getThisIdentifier()));

    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                                           testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                                           testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                                           testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                                           testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                                           testUtils::TestSubSubEnum2::getThisIdentifier()));

    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                            testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(testEnvironment.m_typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                                            testUtils::TestSubSubEnum1::getThisIdentifier()));
}

TEST(TypeSystemTest, getAllSubTypes) {
    testUtils::TestEnvironment testEnvironment;
    EXPECT_TRUE(
        testUtils::areEqualSets(testEnvironment.m_typeSystem.getAllSubtypes(testUtils::TestType::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllSubtypes(testUtils::TestSubSubEnum1::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestSubSubEnum1::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllSubtypes(testUtils::TestSubEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestSubEnum::getThisIdentifier(),
                                          testUtils::TestSubSubEnum1::getThisIdentifier(),
                                          testUtils::TestSubSubEnum2::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllSubtypes(testUtils::TestEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{
            testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier(),
            testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}));
}

TEST(TypeSystemTest, getAllSupertypes) {
    testUtils::TestEnvironment testEnvironment;

    EXPECT_TRUE(
        testUtils::areEqualSets(testEnvironment.m_typeSystem.getAllSupertypes(testUtils::TestType::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(testEnvironment.m_typeSystem.getAllSupertypes(testUtils::TestEnum::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllSupertypes(testUtils::TestSubEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                          testUtils::TestSubEnum::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllSupertypes(testUtils::TestSubSubEnum1::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                          testUtils::TestSubEnum::getThisIdentifier(),
                                          testUtils::TestSubSubEnum1::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllSupertypes(testUtils::TestSubSubEnum2::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                          testUtils::TestSubEnum::getThisIdentifier(),
                                          testUtils::TestSubSubEnum2::getThisIdentifier()}));
}

TEST(TypeSystemTest, addAllRelatedTypes) {
    testUtils::TestEnvironment testEnvironment;

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllRelatedTypes(testUtils::TestType::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllRelatedTypes(testUtils::TestEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{
            testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier(),
            testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllRelatedTypes(testUtils::TestSubEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{
            testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier(),
            testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllRelatedTypes(testUtils::TestSubSubEnum1::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                          testUtils::TestSubEnum::getThisIdentifier(),
                                          testUtils::TestSubSubEnum1::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        testEnvironment.m_typeSystem.getAllRelatedTypes(testUtils::TestSubSubEnum2::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                          testUtils::TestSubEnum::getThisIdentifier(),
                                          testUtils::TestSubSubEnum2::getThisIdentifier()}));
}
