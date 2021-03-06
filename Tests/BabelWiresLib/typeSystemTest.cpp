#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(TypeSystemTest, isSubTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

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

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

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

TEST(TypeSystemTest, addAllSubTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSubtypes(testUtils::TestType::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSubtypes(testUtils::TestSubSubEnum1::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestSubSubEnum1::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSubtypes(testUtils::TestSubEnum::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum2::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSubtypes(testUtils::TestEnum::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum2::getThisIdentifier()}));
    }
}

TEST(TypeSystemTest, addAllSupertypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSupertypes(testUtils::TestType::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSupertypes(testUtils::TestEnum::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSupertypes(testUtils::TestSubEnum::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSupertypes(testUtils::TestSubSubEnum1::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum1::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllSupertypes(testUtils::TestSubSubEnum2::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum2::getThisIdentifier()}));
    }
}

TEST(TypeSystemTest, addAllRelatedTypes) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllRelatedTypes(testUtils::TestType::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestType::getThisIdentifier()}));
    }
    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllRelatedTypes(testUtils::TestEnum::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum2::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllRelatedTypes(testUtils::TestSubEnum::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum2::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllRelatedTypes(testUtils::TestSubSubEnum1::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum1::getThisIdentifier()}));
    }

    {
        babelwires::TypeSystem::TypeIdSet types;
        typeSystem.addAllRelatedTypes(testUtils::TestSubSubEnum2::getThisIdentifier(), types);
        EXPECT_TRUE(testUtils::areEqualSets(
            types, babelwires::TypeSystem::TypeIdSet{testUtils::TestEnum::getThisIdentifier(),
                                                     testUtils::TestSubEnum::getThisIdentifier(),
                                                     testUtils::TestSubSubEnum2::getThisIdentifier()}));
    }
}
