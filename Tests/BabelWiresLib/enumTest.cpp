#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumWithCppEnum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(EnumTest, basic) {
    testUtils::TestEnum testEnum;

    EXPECT_TRUE(testUtils::areEqualSets(testEnum.getValueSet(), babelwires::EnumType::ValueSet{"Foo", "Bar", "Erm", "Oom", "Boo"}));

    EXPECT_EQ(testEnum.getIndexOfDefaultValue(), 1);

    EXPECT_EQ(testEnum.getIndexFromIdentifier("Foo"), 0);
    EXPECT_EQ(testEnum.getIndexFromIdentifier("Bar"), 1);
    EXPECT_EQ(testEnum.getIndexFromIdentifier("Erm"), 2);
    EXPECT_EQ(testEnum.getIdentifierFromIndex(0), "Foo");
    EXPECT_EQ(testEnum.getIdentifierFromIndex(1), "Bar");
    EXPECT_EQ(testEnum.getIdentifierFromIndex(2), "Erm");
    EXPECT_EQ(testEnum.getIdentifierFromIndex(0).getDiscriminator(), 1);
    EXPECT_EQ(testEnum.getIdentifierFromIndex(1).getDiscriminator(), 2);
    EXPECT_EQ(testEnum.getIdentifierFromIndex(2).getDiscriminator(), 3);

    EXPECT_TRUE(testEnum.isAValue("Foo"));
    EXPECT_TRUE(testEnum.isAValue("Bar"));
    EXPECT_TRUE(testEnum.isAValue("Erm"));
    EXPECT_TRUE(testEnum.isAValue("Oom"));
    EXPECT_TRUE(testEnum.isAValue("Boo"));
    EXPECT_FALSE(testEnum.isAValue("Flerm"));
}

#define TEST_ENUM_VALUES(X)                                                                                            \
    X(Foo, "Foo value", "00000000-1111-2222-3333-444444444444")                                                        \
    X(Bar, "Bar value", "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee")                                                        \
    X(Erm, "Erm value", "00000000-1111-2222-dddd-eeeeeeeeeeee")

ENUM_DEFINE_ENUM_VALUE_SOURCE(TEST_ENUM_VALUES);

TEST(EnumTest, enumWithCppEnum) {
    struct TestEnum : babelwires::EnumType {
        TestEnum()
            : EnumType(ENUM_IDENTIFIER_VECTOR(TEST_ENUM_VALUES), 1) {}

        static babelwires::PrimitiveTypeId getThisIdentifier() {
            return babelwires::IdentifierRegistry::write()->addMediumIdWithMetadata(
                                           "TestEnum", "TestEnum", "aaaaaaaa-1111-2222-3333-444444444444",
                                           babelwires::IdentifierRegistry::Authority::isAuthoritative);
        }
        babelwires::TypeRef getTypeRef() const {
            return getThisIdentifier();
        }

        ENUM_DEFINE_CPP_ENUM(TEST_ENUM_VALUES);
    };

    testUtils::TestLog log;
    babelwires::IdentifierRegistryScope fieldRegistry;
    TestEnum testEnum;

    EXPECT_EQ(testEnum.getValueFromIdentifier("Foo"), TestEnum::Value::Foo);
    EXPECT_EQ(testEnum.getValueFromIdentifier("Bar"), TestEnum::Value::Bar);
    EXPECT_EQ(testEnum.getValueFromIdentifier("Erm"), TestEnum::Value::Erm);
    EXPECT_EQ(testEnum.getIdentifierFromValue(TestEnum::Value::Foo), "Foo");
    EXPECT_EQ(testEnum.getIdentifierFromValue(TestEnum::Value::Bar), "Bar");
    EXPECT_EQ(testEnum.getIdentifierFromValue(TestEnum::Value::Erm), "Erm");
    EXPECT_EQ(testEnum.getIdentifierFromValue(TestEnum::Value::Foo).getDiscriminator(), 1);
    EXPECT_EQ(testEnum.getIdentifierFromValue(TestEnum::Value::Bar).getDiscriminator(), 1);
    EXPECT_EQ(testEnum.getIdentifierFromValue(TestEnum::Value::Erm).getDiscriminator(), 1);
}


TEST(EnumTest, createValue) {
    babelwires::TypeSystem typeSystem;
    testUtils::TestEnum testEnum;
    
    auto [valueHolder, value] = testEnum.createValue(typeSystem);
    EXPECT_TRUE(valueHolder);
    auto enumValue = value.as<babelwires::EnumValue>();
    EXPECT_TRUE(enumValue);
    EXPECT_TRUE(testEnum.isValidValue(typeSystem, value));
    EXPECT_EQ(enumValue->get(), "Bar");

    enumValue->set("Foo");
    EXPECT_TRUE(testEnum.isValidValue(typeSystem, value));
    enumValue->set("Flerm");
    EXPECT_FALSE(testEnum.isValidValue(typeSystem, value));
}

TEST(EnumTest, subEnum) {
    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry<testUtils::TestEnum>();
    // Have to register Sub-Enums in the TypeSystem or they don't work.
    typeSystem.addEntry<testUtils::TestSubEnum>();

    const auto& testEnum = typeSystem.getEntryByType<testUtils::TestEnum>();
    const auto& testSubEnum = typeSystem.getEntryByType<testUtils::TestSubEnum>();

    auto [valueHolder, value] = testSubEnum.createValue(typeSystem);
    EXPECT_TRUE(valueHolder);
    auto enumValue = value.as<babelwires::EnumValue>();
    EXPECT_TRUE(enumValue);
    EXPECT_TRUE(testEnum.isValidValue(typeSystem, value));
    EXPECT_TRUE(testSubEnum.isValidValue(typeSystem, value));

    enumValue->set("Foo");
    EXPECT_TRUE(testEnum.isValidValue(typeSystem, value));
    EXPECT_FALSE(testSubEnum.isValidValue(typeSystem, value));
    
    enumValue->set("Flerm");
    EXPECT_FALSE(testEnum.isValidValue(typeSystem, value));
    EXPECT_FALSE(testSubEnum.isValidValue(typeSystem, value));
}