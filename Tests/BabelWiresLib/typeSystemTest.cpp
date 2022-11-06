#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

namespace {
    void addTestTypes(babelwires::TypeSystem& typeSystem) {
        typeSystem.addEntry(std::make_unique<testUtils::TestType>());
        typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
        typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
        typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
        typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

        typeSystem.addRelatedTypes(testUtils::TestEnum::getThisIdentifier(),
                                   {{}, {testUtils::TestSubSubEnum1::getThisIdentifier()}});
        typeSystem.addRelatedTypes(testUtils::TestSubSubEnum2::getThisIdentifier(),
                                   {{testUtils::TestEnum::getThisIdentifier()}, {}});
        typeSystem.addRelatedTypes(
            testUtils::TestSubEnum::getThisIdentifier(),
            {{testUtils::TestEnum::getThisIdentifier()},
             {testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}});
    }
} // namespace

TEST(TypeSystemTest, isSubTypes) {
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
