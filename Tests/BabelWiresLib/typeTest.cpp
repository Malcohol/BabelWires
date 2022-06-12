#include <gtest/gtest.h>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testValueAndType.hpp>

#include <Tests/TestUtils/equalSets.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

TEST(TypeTest, getParent) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

    const auto& testType = typeSystem.getRegisteredEntry(testUtils::TestType::getThisIdentifier());
    const auto& testEnum = typeSystem.getRegisteredEntry(testUtils::TestEnum::getThisIdentifier());
    const auto& testSubEnum = typeSystem.getRegisteredEntry(testUtils::TestSubEnum::getThisIdentifier());
    const auto& testSubSubEnum1 = typeSystem.getRegisteredEntry(testUtils::TestSubSubEnum1::getThisIdentifier());
    const auto& testSubSubEnum2 = typeSystem.getRegisteredEntry(testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_FALSE(testType.getParentTypeId());
    EXPECT_FALSE(testEnum.getParentTypeId());
    EXPECT_EQ(testSubEnum.getParentTypeId(), testUtils::TestEnum::getThisIdentifier());
    EXPECT_EQ(testSubSubEnum1.getParentTypeId(), testUtils::TestSubEnum::getThisIdentifier());
    EXPECT_EQ(testSubSubEnum2.getParentTypeId(), testUtils::TestSubEnum::getThisIdentifier());

    EXPECT_EQ(testType.getParent(), nullptr);
    EXPECT_EQ(testEnum.getParent(), nullptr);
    EXPECT_EQ(testSubEnum.getParent(), &testEnum);
    EXPECT_EQ(testSubSubEnum1.getParent(), &testSubEnum);
    EXPECT_EQ(testSubSubEnum2.getParent(), &testSubEnum);
}

TEST(TypeTest, getChildren) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

    const auto& testType = typeSystem.getRegisteredEntry(testUtils::TestType::getThisIdentifier());
    const auto& testEnum = typeSystem.getRegisteredEntry(testUtils::TestEnum::getThisIdentifier());
    const auto& testSubEnum = typeSystem.getRegisteredEntry(testUtils::TestSubEnum::getThisIdentifier());
    const auto& testSubSubEnum1 = typeSystem.getRegisteredEntry(testUtils::TestSubSubEnum1::getThisIdentifier());
    const auto& testSubSubEnum2 = typeSystem.getRegisteredEntry(testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_EQ(testType.getChildren().size(), 0);
    EXPECT_TRUE(testUtils::areEqualSets(testEnum.getChildren(), std::vector<const babelwires::Type*>{&testSubEnum}));
    EXPECT_TRUE(testUtils::areEqualSets(testSubEnum.getChildren(), std::vector<const babelwires::Type*>{&testSubSubEnum1, &testSubSubEnum2}));
    EXPECT_EQ(testSubSubEnum1.getChildren().size(), 0);
    EXPECT_EQ(testSubSubEnum2.getChildren().size(), 0);  
}

TEST(TypeTest, isSubType) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestType>());
    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum1>());
    typeSystem.addEntry(std::make_unique<testUtils::TestSubSubEnum2>());

    const auto& testType = typeSystem.getRegisteredEntry(testUtils::TestType::getThisIdentifier());
    const auto& testEnum = typeSystem.getRegisteredEntry(testUtils::TestEnum::getThisIdentifier());
    const auto& testSubEnum = typeSystem.getRegisteredEntry(testUtils::TestSubEnum::getThisIdentifier());
    const auto& testSubSubEnum1 = typeSystem.getRegisteredEntry(testUtils::TestSubSubEnum1::getThisIdentifier());
    const auto& testSubSubEnum2 = typeSystem.getRegisteredEntry(testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_TRUE(testType.isSubType(testType));
    EXPECT_TRUE(testEnum.isSubType(testEnum));
    EXPECT_TRUE(testSubEnum.isSubType(testSubEnum));
    EXPECT_TRUE(testSubSubEnum1.isSubType(testSubSubEnum1));
    EXPECT_TRUE(testSubSubEnum2.isSubType(testSubSubEnum2));

    EXPECT_FALSE(testType.isSubType(testEnum));
    EXPECT_FALSE(testEnum.isSubType(testType));

    EXPECT_TRUE(testSubEnum.isSubType(testEnum));
    EXPECT_TRUE(testSubSubEnum1.isSubType(testEnum));
    EXPECT_TRUE(testSubSubEnum2.isSubType(testEnum));
    EXPECT_TRUE(testSubSubEnum1.isSubType(testSubEnum));
    EXPECT_TRUE(testSubSubEnum2.isSubType(testSubEnum));

    EXPECT_FALSE(testSubEnum.isSubType(testSubSubEnum1));
    EXPECT_FALSE(testSubEnum.isSubType(testSubSubEnum2));
    EXPECT_FALSE(testSubSubEnum2.isSubType(testSubSubEnum1));
    EXPECT_FALSE(testSubSubEnum1.isSubType(testSubSubEnum2));
}

TEST(TypeTest, typeAndValue)
{
    // This mostly just exercises the API.

    testUtils::TestType testType;
    auto value = testType.createValue();
    EXPECT_TRUE(value);
    EXPECT_TRUE(value->as<testUtils::TestValue>());
    EXPECT_TRUE(value->isValid(testType));

    testUtils::TestEnum testEnum;
    EXPECT_FALSE(value->isValid(testEnum));
}
