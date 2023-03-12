#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/addBlank.hpp>
#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Enums/enumWithCppEnum.hpp>
#include <BabelWiresLib/TypeSystem/enumValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(AddBlank, ensureBlankValue) {
    testUtils::TestLog log;

    babelwires::Enum::EnumValues values{"Foo", "Bar"};
    babelwires::Enum::EnumValues hasBlank = babelwires::AddBlank::ensureBlankValue(values);
    EXPECT_EQ(hasBlank.size(), 3);
    EXPECT_EQ(hasBlank[0], "Foo");
    EXPECT_EQ(hasBlank[1], "Bar");
    EXPECT_EQ(hasBlank[2], babelwires::AddBlank::getBlankValue());
}

TEST(AddBlank, ensureBlankValueEmpty) {
    testUtils::TestLog log;

    babelwires::Enum::EnumValues empty;
    babelwires::Enum::EnumValues justBlank = babelwires::AddBlank::ensureBlankValue(empty);
    EXPECT_EQ(justBlank.size(), 1);
    EXPECT_EQ(justBlank[0], babelwires::AddBlank::getBlankValue());
}

TEST(AddBlank, constructType) {
    testUtils::TestLog log;

    babelwires::TypeSystem typeSystem;
    const testUtils::TestEnum* const testEnum = typeSystem.addEntry<testUtils::TestEnum>();

    babelwires::AddBlank addBlank;
    const babelwires::Type* const newType =
        addBlank.getOrConstructType(typeSystem, {{testUtils::TestEnum::getThisIdentifier()}});
    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(), babelwires::TypeRef(babelwires::AddBlank::getThisIdentifier(),
                                                         {{testUtils::TestEnum::getThisIdentifier()}}));

    const babelwires::Enum* const newEnum = newType->as<babelwires::Enum>();
    ASSERT_NE(newEnum, nullptr);

    EXPECT_EQ(newEnum->getEnumValues().size(), testEnum->getEnumValues().size() + 1);
    for (int i = 0; i < testEnum->getEnumValues().size(); ++i) {
        EXPECT_EQ(newEnum->getEnumValues()[i], testEnum->getEnumValues()[i]);
    }
    EXPECT_EQ(newEnum->getEnumValues()[newEnum->getEnumValues().size() - 1], babelwires::AddBlank::getBlankValue());
    EXPECT_EQ(newEnum->getIndexOfDefaultValue(), testEnum->getIndexOfDefaultValue());
}

TEST(AddBlank, idempotency) {
    testUtils::TestLog log;

    babelwires::TypeSystem typeSystem;
    const testUtils::TestEnum* const testEnum = typeSystem.addEntry<testUtils::TestEnum>();
    typeSystem.addTypeConstructor<babelwires::AddBlank>();

    babelwires::AddBlank addBlank;
    const babelwires::Type* const newType =
        addBlank.getOrConstructType(typeSystem, {{babelwires::TypeRef(babelwires::AddBlank::getThisIdentifier(),
                                                                      {{testUtils::TestEnum::getThisIdentifier()}})}});

    ASSERT_NE(newType, nullptr);
    EXPECT_EQ(newType->getTypeRef(),
              babelwires::TypeRef(babelwires::AddBlank::getThisIdentifier(),
                                  {{babelwires::TypeRef(babelwires::AddBlank::getThisIdentifier(),
                                                        {{testUtils::TestEnum::getThisIdentifier()}})}}));

    const babelwires::Enum* const newEnum = newType->as<babelwires::Enum>();
    ASSERT_NE(newEnum, nullptr);

    EXPECT_EQ(newEnum->getEnumValues().size(), testEnum->getEnumValues().size() + 1);
    for (int i = 0; i < testEnum->getEnumValues().size(); ++i) {
        EXPECT_EQ(newEnum->getEnumValues()[i], testEnum->getEnumValues()[i]);
    }
    EXPECT_EQ(newEnum->getEnumValues()[newEnum->getEnumValues().size() - 1], babelwires::AddBlank::getBlankValue());
    EXPECT_EQ(newEnum->getIndexOfDefaultValue(), testEnum->getIndexOfDefaultValue());
}

TEST(AddBlank, compareSubtype) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    testUtils::addTestEnumTypes(typeSystem);
    typeSystem.addTypeConstructor<babelwires::AddBlank>();

    babelwires::TypeRef addBlankToSubEnum(babelwires::AddBlank::getThisIdentifier(),
                                          {{testUtils::TestSubEnum::getThisIdentifier()}});

    babelwires::TypeRef addBlankToSubSubEnum1(babelwires::AddBlank::getThisIdentifier(),
                                              {{testUtils::TestSubSubEnum1::getThisIdentifier()}});

    babelwires::TypeRef addBlankToSubSubEnum2(babelwires::AddBlank::getThisIdentifier(),
                                              {{testUtils::TestSubSubEnum2::getThisIdentifier()}});

    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubEnum, addBlankToSubEnum), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubEnum, addBlankToSubSubEnum1),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubEnum, addBlankToSubSubEnum2),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubSubEnum1, addBlankToSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubSubEnum1, addBlankToSubSubEnum1),
              babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubSubEnum1, addBlankToSubSubEnum2),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubSubEnum2, addBlankToSubEnum), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubSubEnum2, addBlankToSubSubEnum1),
              babelwires::SubtypeOrder::IsUnrelated);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubSubEnum2, addBlankToSubSubEnum2),
              babelwires::SubtypeOrder::IsEquivalent);

    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubEnum, testUtils::TestSubEnum::getThisIdentifier()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubEnum, testUtils::TestSubSubEnum1::getThisIdentifier()),
              babelwires::SubtypeOrder::IsSupertype);
    EXPECT_EQ(typeSystem.compareSubtype(addBlankToSubEnum, testUtils::TestEnum::getThisIdentifier()),
              babelwires::SubtypeOrder::IsUnrelated);

    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestSubEnum::getThisIdentifier(), addBlankToSubEnum),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestSubSubEnum1::getThisIdentifier(), addBlankToSubEnum),
              babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(typeSystem.compareSubtype(testUtils::TestEnum::getThisIdentifier(), addBlankToSubEnum),
              babelwires::SubtypeOrder::IsUnrelated);
}
