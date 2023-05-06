#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Enum/addBlankToEnum.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumWithCppEnum.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(AddBlankToEnum, ensureBlankValue) {
    testUtils::TestLog log;

    babelwires::EnumType::ValueSet values{"Foo", "Bar"};
    babelwires::EnumType::ValueSet hasBlank = babelwires::AddBlankToEnum::ensureBlankValue(values);
    EXPECT_EQ(hasBlank.size(), 3);
    EXPECT_EQ(hasBlank[0], "Foo");
    EXPECT_EQ(hasBlank[1], "Bar");
    EXPECT_EQ(hasBlank[2], babelwires::AddBlankToEnum::getBlankValue());
}

TEST(AddBlankToEnum, ensureBlankValueEmpty) {
    testUtils::TestLog log;

    babelwires::EnumType::ValueSet empty;
    babelwires::EnumType::ValueSet justBlank = babelwires::AddBlankToEnum::ensureBlankValue(empty);
    EXPECT_EQ(justBlank.size(), 1);
    EXPECT_EQ(justBlank[0], babelwires::AddBlankToEnum::getBlankValue());
}


TEST(AddBlankToEnum, constructType) {
    testUtils::TestEnvironment testEnvironment;
    const testUtils::TestEnum& testEnum = testEnvironment.m_typeSystem.getEntryByType<testUtils::TestEnum>();

    babelwires::AddBlankToEnum addBlankToEnum;
    const babelwires::Type* const newType =
        addBlankToEnum.getOrConstructType(testEnvironment.m_typeSystem, babelwires::TypeConstructorArguments{{testUtils::TestEnum::getThisIdentifier()}});
    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(), babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                                         testUtils::TestEnum::getThisIdentifier()));

    const babelwires::EnumType* const newEnum = newType->as<babelwires::EnumType>();
    ASSERT_NE(newEnum, nullptr);

    EXPECT_EQ(newEnum->getValueSet().size(), testEnum.getValueSet().size() + 1);
    for (int i = 0; i < testEnum.getValueSet().size(); ++i) {
        EXPECT_EQ(newEnum->getValueSet()[i], testEnum.getValueSet()[i]);
    }
    EXPECT_EQ(newEnum->getValueSet()[newEnum->getValueSet().size() - 1], babelwires::AddBlankToEnum::getBlankValue());
    EXPECT_EQ(newEnum->getIndexOfDefaultValue(), testEnum.getIndexOfDefaultValue());
}

TEST(AddBlankToEnum, idempotency) {
    testUtils::TestEnvironment testEnvironment;
    const testUtils::TestEnum& testEnum = testEnvironment.m_typeSystem.getEntryByType<testUtils::TestEnum>();

    babelwires::AddBlankToEnum addBlankToEnum;
    const babelwires::Type* const newType =
        addBlankToEnum.getOrConstructType(testEnvironment.m_typeSystem, babelwires::TypeConstructorArguments{{babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                                                      testUtils::TestEnum::getThisIdentifier())}});

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(),
              babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                  babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                                        testUtils::TestEnum::getThisIdentifier())));

    const babelwires::EnumType* const newEnum = newType->as<babelwires::EnumType>();
    ASSERT_NE(newEnum, nullptr);

    EXPECT_EQ(newEnum->getValueSet().size(), testEnum.getValueSet().size() + 1);
    for (int i = 0; i < testEnum.getValueSet().size(); ++i) {
        EXPECT_EQ(newEnum->getValueSet()[i], testEnum.getValueSet()[i]);
    }
    EXPECT_EQ(newEnum->getValueSet()[newEnum->getValueSet().size() - 1], babelwires::AddBlankToEnum::getBlankValue());
    EXPECT_EQ(newEnum->getIndexOfDefaultValue(), testEnum.getIndexOfDefaultValue());
}

TEST(AddBlankToEnum, compareSubtype) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeRef addBlankToEnumToSubEnum(babelwires::AddBlankToEnum::getThisIdentifier(),
                                          testUtils::TestSubEnum::getThisIdentifier());

    babelwires::TypeRef addBlankToEnumToSubSubEnum1(babelwires::AddBlankToEnum::getThisIdentifier(),
                                              testUtils::TestSubSubEnum1::getThisIdentifier());

    babelwires::TypeRef addBlankToEnumToSubSubEnum2(babelwires::AddBlankToEnum::getThisIdentifier(),
                                              testUtils::TestSubSubEnum2::getThisIdentifier());

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubEnum, addBlankToEnumToSubEnum), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubEnum, addBlankToEnumToSubSubEnum1),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubEnum, addBlankToEnumToSubSubEnum2),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubSubEnum1, addBlankToEnumToSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubSubEnum1, addBlankToEnumToSubSubEnum1),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubSubEnum1, addBlankToEnumToSubSubEnum2),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubSubEnum2, addBlankToEnumToSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubSubEnum2, addBlankToEnumToSubSubEnum1),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubSubEnum2, addBlankToEnumToSubSubEnum2),
              babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubEnum, testUtils::TestSubEnum::getThisIdentifier()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubEnum, testUtils::TestSubSubEnum1::getThisIdentifier()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(addBlankToEnumToSubEnum, testUtils::TestEnum::getThisIdentifier()),
              babelwires::SubtypeOrder::IsUnrelated);

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testUtils::TestSubEnum::getThisIdentifier(), addBlankToEnumToSubEnum),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testUtils::TestSubSubEnum1::getThisIdentifier(), addBlankToEnumToSubEnum),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(testUtils::TestEnum::getThisIdentifier(), addBlankToEnumToSubEnum),
              babelwires::SubtypeOrder::IsUnrelated);
}
