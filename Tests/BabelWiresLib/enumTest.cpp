#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Enums/enumWithCppEnum.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/enumValue.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testIdentifiers.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(EnumTest, basic) {
    testUtils::TestEnum testEnum;

    EXPECT_TRUE(testUtils::areEqualSets(testEnum.getEnumValues(), babelwires::Enum::EnumValues{"Foo", "Bar", "Erm", "Oom", "Boo"}));

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
    struct TestEnum : babelwires::Enum {
        TestEnum()
            : Enum(getThisIdentifier(), 1, ENUM_IDENTIFIER_VECTOR(TEST_ENUM_VALUES), 1) {}

        static babelwires::LongIdentifier getThisIdentifier() {
            return babelwires::IdentifierRegistry::write()->addLongIdentifierWithMetadata(
                                           "TestEnum", "TestEnum", "aaaaaaaa-1111-2222-3333-444444444444",
                                           babelwires::IdentifierRegistry::Authority::isAuthoritative);
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
    testUtils::TestEnum testEnum;
    
    auto value = testEnum.createValue();
    EXPECT_TRUE(value);
    auto enumValue = value->as<babelwires::EnumValue>();
    EXPECT_TRUE(enumValue);
    EXPECT_TRUE(value->isValid(testEnum));
    EXPECT_EQ(enumValue->get(), "Bar");

    enumValue->set("Foo");
    EXPECT_TRUE(value->isValid(testEnum));
    enumValue->set("Flerm");
    EXPECT_FALSE(value->isValid(testEnum));
}

TEST(EnumTest, subEnum) {
    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry(std::make_unique<testUtils::TestEnum>());
    // Have to register Sub-Enums in the TypeSystem or they don't work.
    typeSystem.addEntry(std::make_unique<testUtils::TestSubEnum>());

    const auto& testEnum = typeSystem.getRegisteredEntry(testUtils::TestEnum::getThisIdentifier());
    const auto& testSubEnum = typeSystem.getRegisteredEntry(testUtils::TestSubEnum::getThisIdentifier());

    auto value = testSubEnum.createValue();
    EXPECT_TRUE(value);
    auto enumValue = value->as<babelwires::EnumValue>();
    EXPECT_TRUE(enumValue);
    EXPECT_TRUE(value->isValid(testEnum));
    EXPECT_TRUE(value->isValid(testSubEnum));

    enumValue->set("Foo");
    EXPECT_TRUE(value->isValid(testEnum));
    EXPECT_FALSE(value->isValid(testSubEnum));
    
    enumValue->set("Flerm");
    EXPECT_FALSE(value->isValid(testEnum));
    EXPECT_FALSE(value->isValid(testSubEnum));
}