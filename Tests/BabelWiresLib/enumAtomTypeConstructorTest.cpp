#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/Types/Enum/enumAtomTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(EnumAtomTypeConstructor, construct) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ShortId foo = testUtils::getTestRegisteredIdentifier("foo");
    babelwires::EnumValue enumValue(foo);

    babelwires::EnumAtomTypeConstructor constructor;

    const babelwires::Type* const newType = constructor.tryGetOrConstructType(
        testEnvironment.m_typeSystem, babelwires::TypeConstructorArguments{{}, {enumValue}});

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(),
              babelwires::TypeRef(babelwires::EnumAtomTypeConstructor::getThisIdentifier(), enumValue));

    const babelwires::EnumType* const enumType = newType->as<babelwires::EnumType>();
    ASSERT_NE(enumType, nullptr);

    const auto& valueSet = enumType->getValueSet();
    EXPECT_EQ(valueSet.size(), 1);
    EXPECT_EQ(valueSet[0], foo);
}

TEST(EnumAtomTypeConstructor, makeTypeRef) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ShortId foo = testUtils::getTestRegisteredIdentifier("foo");
    babelwires::EnumValue enumValue(foo);

    babelwires::TypeRef typeRef = babelwires::EnumAtomTypeConstructor::makeTypeRef(enumValue);

    const babelwires::Type* const newType = typeRef.tryResolve(testEnvironment.m_typeSystem);

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(),
              babelwires::TypeRef(babelwires::EnumAtomTypeConstructor::getThisIdentifier(), enumValue));

    const babelwires::EnumType* const enumType = newType->as<babelwires::EnumType>();
    ASSERT_NE(enumType, nullptr);

    const auto& valueSet = enumType->getValueSet();
    EXPECT_EQ(valueSet.size(), 1);
    EXPECT_EQ(valueSet[0], foo);
}

TEST(EnumAtomTypeConstructor, fail) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EnumAtomTypeConstructor constructor;

    EXPECT_THROW(
        constructor.getOrConstructType(testEnvironment.m_typeSystem, babelwires::TypeConstructorArguments{}),
        babelwires::TypeSystemException);
    EXPECT_THROW(constructor.getOrConstructType(testEnvironment.m_typeSystem,
                                                   babelwires::TypeConstructorArguments{{}, {babelwires::IntValue()}}),
                 babelwires::TypeSystemException);
    EXPECT_THROW(constructor.getOrConstructType(
                     testEnvironment.m_typeSystem,
                     babelwires::TypeConstructorArguments{{babelwires::DefaultIntType::getThisIdentifier()},
                                                          {babelwires::IntValue()}}),
                 babelwires::TypeSystemException);
}