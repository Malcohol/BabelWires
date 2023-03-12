#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/addBlankToEnum.hpp>
#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Enums/enumWithCppEnum.hpp>
#include <BabelWiresLib/TypeSystem/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(AddBlankToEnum, ensureBlankValue) {
    testUtils::TestLog log;

    babelwires::Enum::EnumValues values{"Foo", "Bar"};
    babelwires::Enum::EnumValues hasBlank = babelwires::AddBlankToEnum::ensureBlankValue(values);
    EXPECT_EQ(hasBlank.size(), 3);
    EXPECT_EQ(hasBlank[0], "Foo");
    EXPECT_EQ(hasBlank[1], "Bar");
    EXPECT_EQ(hasBlank[2], babelwires::AddBlankToEnum::getBlankValue());
}

TEST(AddBlankToEnum, ensureBlankValueEmpty) {
    testUtils::TestLog log;

    babelwires::Enum::EnumValues empty;
    babelwires::Enum::EnumValues justBlank = babelwires::AddBlankToEnum::ensureBlankValue(empty);
    EXPECT_EQ(justBlank.size(), 1);
    EXPECT_EQ(justBlank[0], babelwires::AddBlankToEnum::getBlankValue());
}

TEST(AddBlankToEnum, constructType) {
    testUtils::TestLog log;

    babelwires::TypeSystem typeSystem;
    const testUtils::TestEnum* const testEnum = typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AddBlankToEnum addBlankToEnum;
    const babelwires::Type* const newType =
        addBlankToEnum.getOrConstructType(typeSystem, {{testUtils::TestEnum::getThisIdentifier()}});
    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(), babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                                         {{testUtils::TestEnum::getThisIdentifier()}}));

    const babelwires::Enum* const newEnum = newType->as<babelwires::Enum>();
    ASSERT_NE(newEnum, nullptr);

    EXPECT_EQ(newEnum->getEnumValues().size(), testEnum->getEnumValues().size() + 1);
    for (int i = 0; i < testEnum->getEnumValues().size(); ++i) {
        EXPECT_EQ(newEnum->getEnumValues()[i], testEnum->getEnumValues()[i]);
    }
    EXPECT_EQ(newEnum->getEnumValues()[newEnum->getEnumValues().size() - 1], babelwires::AddBlankToEnum::getBlankValue());
    EXPECT_EQ(newEnum->getIndexOfDefaultValue(), testEnum->getIndexOfDefaultValue());
}

TEST(AddBlankToEnum, idempotency) {
    testUtils::TestLog log;

    babelwires::TypeSystem typeSystem;
    const testUtils::TestEnum* const testEnum = typeSystem.addEntry<testUtils::TestEnum>();
    typeSystem.addTypeConstructor<babelwires::AddBlankToEnum>();

    babelwires::AddBlankToEnum addBlankToEnum;
    const babelwires::Type* const newType =
        addBlankToEnum.getOrConstructType(typeSystem, {{babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                                                      {{testUtils::TestEnum::getThisIdentifier()}})}});

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(),
              babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                  {{babelwires::TypeRef(babelwires::AddBlankToEnum::getThisIdentifier(),
                                                        {{testUtils::TestEnum::getThisIdentifier()}})}}));

    const babelwires::Enum* const newEnum = newType->as<babelwires::Enum>();
    ASSERT_NE(newEnum, nullptr);

    EXPECT_EQ(newEnum->getEnumValues().size(), testEnum->getEnumValues().size() + 1);
    for (int i = 0; i < testEnum->getEnumValues().size(); ++i) {
        EXPECT_EQ(newEnum->getEnumValues()[i], testEnum->getEnumValues()[i]);
    }
    EXPECT_EQ(newEnum->getEnumValues()[newEnum->getEnumValues().size() - 1], babelwires::AddBlankToEnum::getBlankValue());
    EXPECT_EQ(newEnum->getIndexOfDefaultValue(), testEnum->getIndexOfDefaultValue());
}

TEST(AddBlankToEnum, compareSubtype) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    testUtils::addTestEnumTypes(typeSystem);
    typeSystem.addTypeConstructor<babelwires::AddBlankToEnum>();

    babelwires::TypeRef addBlankToEnumToSubEnum(babelwires::AddBlankToEnum::getThisIdentifier(),
                                          {{testUtils::TestSubEnum::getThisIdentifier()}});

    babelwires::TypeRef addBlankToEnumToSubSubEnum1(babelwires::AddBlankToEnum::getThisIdentifier(),
                                              {{testUtils::TestSubSubEnum1::getThisIdentifier()}});

    babelwires::TypeRef addBlankToEnumToSubSubEnum2(babelwires::AddBlankToEnum::getThisIdentifier(),
                                              {{testUtils::TestSubSubEnum2::getThisIdentifier()}});

    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubEnum, addBlankToEnumToSubEnum), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubEnum, addBlankToEnumToSubSubEnum1),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubEnum, addBlankToEnumToSubSubEnum2),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubSubEnum1, addBlankToEnumToSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubSubEnum1, addBlankToEnumToSubSubEnum1),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubSubEnum1, addBlankToEnumToSubSubEnum2),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubSubEnum2, addBlankToEnumToSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubSubEnum2, addBlankToEnumToSubSubEnum1),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubSubEnum2, addBlankToEnumToSubSubEnum2),
              babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubEnum, testUtils::TestSubEnum::getThisIdentifier()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubEnum, testUtils::TestSubSubEnum1::getThisIdentifier()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToEnumToSubEnum, testUtils::TestEnum::getThisIdentifier()),
              babelwires::SubtypeOrder::IsUnrelated);

    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestSubEnum::getThisIdentifier(), addBlankToEnumToSubEnum),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestSubSubEnum1::getThisIdentifier(), addBlankToEnumToSubEnum),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestEnum::getThisIdentifier(), addBlankToEnumToSubEnum),
              babelwires::SubtypeOrder::IsUnrelated);
}
