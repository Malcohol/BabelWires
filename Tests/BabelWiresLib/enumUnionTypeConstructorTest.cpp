#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumUnionTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(EnumUnionTypeConstructorTest, construct) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EnumUnionTypeConstructor constructor;

    const babelwires::Type* const newType = constructor.tryGetOrConstructType(
        testEnvironment.m_typeSystem,
        babelwires::TypeConstructorArguments{
            {testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier()}});

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(), babelwires::TypeRef(babelwires::EnumUnionTypeConstructor::getThisIdentifier(),
                                                         testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                         testUtils::TestSubSubEnum2::getThisIdentifier()));

    const babelwires::EnumType* const enumType = newType->as<babelwires::EnumType>();
    ASSERT_NE(enumType, nullptr);

    const auto& valueSet = enumType->getValueSet();
    EXPECT_EQ(valueSet.size(), 3);
    EXPECT_EQ(valueSet[0], testUtils::TestEnum::getIdentifierFromValue(testUtils::TestEnum::Value::Bar));
    EXPECT_EQ(valueSet[1], testUtils::TestEnum::getIdentifierFromValue(testUtils::TestEnum::Value::Erm));
    EXPECT_EQ(valueSet[2], testUtils::TestEnum::getIdentifierFromValue(testUtils::TestEnum::Value::Oom));
}

TEST(EnumUnionTypeConstructorTest, makeTypeRef) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef typeRef = babelwires::EnumUnionTypeConstructor::makeTypeRef(
        testUtils::TestSubSubEnum1::getThisIdentifier(), testUtils::TestSubSubEnum2::getThisIdentifier());

    const babelwires::Type* const newType = typeRef.tryResolve(testEnvironment.m_typeSystem);

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(), babelwires::TypeRef(babelwires::EnumUnionTypeConstructor::getThisIdentifier(),
                                                         testUtils::TestSubSubEnum1::getThisIdentifier(),
                                                         testUtils::TestSubSubEnum2::getThisIdentifier()));

    const babelwires::EnumType* const enumType = newType->as<babelwires::EnumType>();
    ASSERT_NE(enumType, nullptr);

    const auto& valueSet = enumType->getValueSet();
    EXPECT_EQ(valueSet.size(), 3);
    EXPECT_EQ(valueSet[0], testUtils::TestEnum::getIdentifierFromValue(testUtils::TestEnum::Value::Bar));
    EXPECT_EQ(valueSet[1], testUtils::TestEnum::getIdentifierFromValue(testUtils::TestEnum::Value::Erm));
    EXPECT_EQ(valueSet[2], testUtils::TestEnum::getIdentifierFromValue(testUtils::TestEnum::Value::Oom));
}

TEST(EnumUnionTypeConstructorTest, throwsOnFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EnumUnionTypeConstructor constructor;

    EXPECT_THROW(constructor.getOrConstructType(testEnvironment.m_typeSystem, babelwires::TypeConstructorArguments{}),
                 babelwires::TypeSystemException);
    EXPECT_THROW(constructor.getOrConstructType(testEnvironment.m_typeSystem,
                                                babelwires::TypeConstructorArguments{{}, {babelwires::IntValue()}}),
                 babelwires::TypeSystemException);
    EXPECT_THROW(constructor.getOrConstructType(
                     testEnvironment.m_typeSystem,
                     babelwires::TypeConstructorArguments{{babelwires::DefaultIntType::getThisIdentifier()}}),
                 babelwires::TypeSystemException);
}