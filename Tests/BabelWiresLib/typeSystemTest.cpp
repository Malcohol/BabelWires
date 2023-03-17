#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testTypeConstructor.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

namespace {
    void addTestTypes(babelwires::TypeSystem& typeSystem) {
        typeSystem.addEntry<testUtils::TestType>();
        testUtils::addTestEnumTypes(typeSystem);
    }
} // namespace

TEST(TypeSystemTest, isSubTypesPrimitives) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);

    EXPECT_TRUE(
        typeSystem.isSubType(testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));
    EXPECT_FALSE(
        typeSystem.isSubType(testUtils::TestType::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()));
    EXPECT_FALSE(
        typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));

    EXPECT_TRUE(
        typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(
        typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                     testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                     testUtils::TestSubSubEnum2::getThisIdentifier()));

    EXPECT_TRUE(
        typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                     testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                     testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                     testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                     testUtils::TestSubEnum::getThisIdentifier()));

    EXPECT_FALSE(
        typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_FALSE(typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(),
                                      testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_FALSE(typeSystem.isSubType(testUtils::TestEnum::getThisIdentifier(),
                                      testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(),
                                      testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_FALSE(typeSystem.isSubType(testUtils::TestSubEnum::getThisIdentifier(),
                                      testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(typeSystem.isSubType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                      testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(typeSystem.isSubType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                      testUtils::TestSubSubEnum1::getThisIdentifier()));
}

TEST(TypeSystemTest, compareSubtypeUnary) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);
    typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();

    babelwires::TypeRef unaryOfSubEnum(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestSubEnum::getThisIdentifier());

    babelwires::TypeRef unaryOfSubSubEnum1(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestSubSubEnum1::getThisIdentifier());

    babelwires::TypeRef unaryOfSubSubEnum2(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestSubSubEnum2::getThisIdentifier());
    
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubEnum, unaryOfSubEnum), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubEnum, unaryOfSubSubEnum1), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubEnum, unaryOfSubSubEnum2), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubSubEnum1, unaryOfSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubSubEnum1, unaryOfSubSubEnum1), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubSubEnum1, unaryOfSubSubEnum2), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubSubEnum2, unaryOfSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubSubEnum2, unaryOfSubSubEnum1), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubSubEnum2, unaryOfSubSubEnum2), babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubEnum, testUtils::TestSubEnum::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubEnum, testUtils::TestSubSubEnum1::getThisIdentifier()), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfSubEnum, testUtils::TestEnum::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);
    
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestSubEnum::getThisIdentifier(), unaryOfSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestSubSubEnum1::getThisIdentifier(), unaryOfSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestEnum::getThisIdentifier(), unaryOfSubEnum), babelwires::SubtypeOrder::IsUnrelated);
}

TEST(TypeSystemTest, compareSubtypeBinary) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);
    typeSystem.addTypeConstructor<testUtils::TestBinaryTypeConstructor>();

    babelwires::TypeRef binaryOfSubEnumSubEnum(testUtils::TestBinaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier());

    babelwires::TypeRef binaryOfEnumSubSubEnum1(testUtils::TestBinaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubSubEnum1::getThisIdentifier());

    babelwires::TypeRef binaryOfSubEnumSubSubEnum2(testUtils::TestBinaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_EQ(typeSystem.compareSubtype(binaryOfSubEnumSubEnum, binaryOfSubEnumSubEnum), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfSubEnumSubEnum, binaryOfEnumSubSubEnum1), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfSubEnumSubEnum, binaryOfSubEnumSubSubEnum2), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfEnumSubSubEnum1, binaryOfSubEnumSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfEnumSubSubEnum1, binaryOfEnumSubSubEnum1), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfEnumSubSubEnum1, binaryOfSubEnumSubSubEnum2), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfSubEnumSubSubEnum2, binaryOfSubEnumSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfSubEnumSubSubEnum2, binaryOfEnumSubSubEnum1), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfSubEnumSubSubEnum2, binaryOfSubEnumSubSubEnum2), babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(typeSystem.compareSubtype(binaryOfSubEnumSubEnum, testUtils::TestSubEnum::getThisIdentifier()), babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestSubEnum::getThisIdentifier(), binaryOfSubEnumSubEnum), babelwires::SubtypeOrder::IsUnrelated);
}

TEST(TypeSystemTest, compareSubtypeComplex) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);
    typeSystem.addTypeConstructor<testUtils::TestUnaryTypeConstructor>();
    typeSystem.addTypeConstructor<testUtils::TestBinaryTypeConstructor>();

    babelwires::TypeRef binaryOfEnumSubSubEnum1(testUtils::TestBinaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubSubEnum1::getThisIdentifier());

    babelwires::TypeRef unaryOfBinaryOfEnumSubSubEnum1(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                       {{binaryOfEnumSubSubEnum1}});

    babelwires::TypeRef binaryOfSubEnumSubEnum(testUtils::TestBinaryTypeConstructor::getThisIdentifier(),
                                       testUtils::TestSubEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier());

    babelwires::TypeRef unaryOfBinaryOfSubEnumSubEnum(testUtils::TestUnaryTypeConstructor::getThisIdentifier(),
                                       {{binaryOfSubEnumSubEnum}});

    EXPECT_EQ(typeSystem.compareSubtype(unaryOfBinaryOfEnumSubSubEnum1, unaryOfBinaryOfSubEnumSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(unaryOfBinaryOfSubEnumSubEnum, unaryOfBinaryOfEnumSubSubEnum1), babelwires::SubtypeOrder::IsSupertype);

    EXPECT_EQ(typeSystem.compareSubtype(unaryOfBinaryOfSubEnumSubEnum, binaryOfEnumSubSubEnum1), babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(binaryOfEnumSubSubEnum1, unaryOfBinaryOfSubEnumSubEnum), babelwires::SubtypeOrder::IsSubtype);
}

TEST(TypeSystemTest, isRelatedTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);

    EXPECT_TRUE(
        typeSystem.isRelatedType(testUtils::TestType::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));
    EXPECT_FALSE(
        typeSystem.isRelatedType(testUtils::TestType::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()));
    EXPECT_FALSE(
        typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(), testUtils::TestType::getThisIdentifier()));

    EXPECT_TRUE(
        typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(), testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                         testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                         testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                         testUtils::TestSubSubEnum2::getThisIdentifier()));

    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                         testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                         testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                         testUtils::TestEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                         testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                         testUtils::TestSubEnum::getThisIdentifier()));

    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                         testUtils::TestSubEnum::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                         testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestEnum::getThisIdentifier(),
                                         testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                         testUtils::TestSubSubEnum1::getThisIdentifier()));
    EXPECT_TRUE(typeSystem.isRelatedType(testUtils::TestSubEnum::getThisIdentifier(),
                                         testUtils::TestSubSubEnum2::getThisIdentifier()));

    EXPECT_FALSE(typeSystem.isRelatedType(testUtils::TestSubSubEnum1::getThisIdentifier(),
                                          testUtils::TestSubSubEnum2::getThisIdentifier()));
    EXPECT_FALSE(typeSystem.isRelatedType(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                          testUtils::TestSubSubEnum1::getThisIdentifier()));
}

TEST(TypeSystemTest, getAllSubTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);

    EXPECT_TRUE(testUtils::areEqualSets(typeSystem.getAllSubtypes(testUtils::TestType::getThisIdentifier()),
                                        babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(typeSystem.getAllSubtypes(testUtils::TestSubSubEnum1::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestSubSubEnum1::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(typeSystem.getAllSubtypes(testUtils::TestSubEnum::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestSubEnum::getThisIdentifier(),
                                                                  testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                                  testUtils::TestSubSubEnum2::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        typeSystem.getAllSubtypes(testUtils::TestEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{
            testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier(),
            testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}));
}

TEST(TypeSystemTest, getAllSupertypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);

    EXPECT_TRUE(testUtils::areEqualSets(typeSystem.getAllSupertypes(testUtils::TestType::getThisIdentifier()),
                                        babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(typeSystem.getAllSupertypes(testUtils::TestEnum::getThisIdentifier()),
                                        babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(typeSystem.getAllSupertypes(testUtils::TestSubEnum::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                                  testUtils::TestSubEnum::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(typeSystem.getAllSupertypes(testUtils::TestSubSubEnum1::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                                  testUtils::TestSubEnum::getThisIdentifier(),
                                                                  testUtils::TestSubSubEnum1::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(typeSystem.getAllSupertypes(testUtils::TestSubSubEnum2::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                                  testUtils::TestSubEnum::getThisIdentifier(),
                                                                  testUtils::TestSubSubEnum2::getThisIdentifier()}));
}

TEST(TypeSystemTest, addAllRelatedTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;
    addTestTypes(typeSystem);

    EXPECT_TRUE(testUtils::areEqualSets(typeSystem.getAllRelatedTypes(testUtils::TestType::getThisIdentifier()),
                                        babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        typeSystem.getAllRelatedTypes(testUtils::TestEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{
            testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier(),
            testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}));

    EXPECT_TRUE(testUtils::areEqualSets(
        typeSystem.getAllRelatedTypes(testUtils::TestSubEnum::getThisIdentifier()),
        babelwires::TypeSystem::TypeIdSet{
            testUtils::TestEnum::getThisIdentifier(), testUtils::TestSubEnum::getThisIdentifier(),
            testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(typeSystem.getAllRelatedTypes(testUtils::TestSubSubEnum1::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                                  testUtils::TestSubEnum::getThisIdentifier(),
                                                                  testUtils::TestSubSubEnum1::getThisIdentifier()}));

    EXPECT_TRUE(
        testUtils::areEqualSets(typeSystem.getAllRelatedTypes(testUtils::TestSubSubEnum2::getThisIdentifier()),
                                babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                                  testUtils::TestSubEnum::getThisIdentifier(),
                                                                  testUtils::TestSubSubEnum2::getThisIdentifier()}));
}
