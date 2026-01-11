#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/Types/Enum/enumAtomTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

TEST(EnumAtomTypeConstructorTest, construct) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ShortId foo = testUtils::getTestRegisteredIdentifier("foo");
    babelwires::EnumValue enumValue(foo);

    babelwires::EnumAtomTypeConstructor constructor;

    const babelwires::TypePtr newType = constructor.tryGetOrConstructType(
        testEnvironment.m_typeSystem, babelwires::TypeConstructorArguments{{}, {enumValue.clone()}});

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeExp(),
              babelwires::TypeExp(babelwires::EnumAtomTypeConstructor::getThisIdentifier(), enumValue.clone()));

    const babelwires::EnumType* const enumType = newType->tryAs<babelwires::EnumType>();
    ASSERT_NE(enumType, nullptr);

    const auto& valueSet = enumType->getValueSet();
    EXPECT_EQ(valueSet.size(), 1);
    EXPECT_EQ(valueSet[0], foo);
}

TEST(EnumAtomTypeConstructorTest, makeTypeExp) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::ShortId foo = testUtils::getTestRegisteredIdentifier("foo");

    babelwires::TypeExp typeExp = babelwires::EnumAtomTypeConstructor::makeTypeExp(babelwires::EnumValue(foo));

    babelwires::TypePtr newType = typeExp.tryResolve(testEnvironment.m_typeSystem);

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeExp(),
              babelwires::TypeExp(babelwires::EnumAtomTypeConstructor::getThisIdentifier(), babelwires::EnumValue(foo)));

    const babelwires::EnumType* const enumType = newType->tryAs<babelwires::EnumType>();
    ASSERT_NE(enumType, nullptr);

    const auto& valueSet = enumType->getValueSet();
    EXPECT_EQ(valueSet.size(), 1);
    EXPECT_EQ(valueSet[0], foo);
}

TEST(EnumAtomTypeConstructorTest, throwsOnFailure) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::EnumAtomTypeConstructor constructor;

    EXPECT_THROW(constructor.getOrConstructType(testEnvironment.m_typeSystem, babelwires::TypeConstructorArguments{}),
                 babelwires::TypeSystemException);
    EXPECT_THROW(constructor.getOrConstructType(testEnvironment.m_typeSystem,
                                                babelwires::TypeConstructorArguments{{}, {babelwires::IntValue()}}),
                 babelwires::TypeSystemException);
    EXPECT_THROW(constructor.getOrConstructType(
                     testEnvironment.m_typeSystem,
                     babelwires::TypeConstructorArguments{{babelwires::DefaultIntType::getThisIdentifier()}, {}}),
                 babelwires::TypeSystemException);
}