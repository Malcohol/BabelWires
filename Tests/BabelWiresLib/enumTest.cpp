#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Enums/enumWithCppEnum.hpp>
#include <BabelWiresLib/Features/Path/fieldNameRegistry.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnum.hpp>
#include <Tests/TestUtils/equalSets.hpp>
#include <Tests/TestUtils/testLog.hpp>

TEST(EnumTest, basic) {
    testUtils::TestEnum testEnum;

    EXPECT_TRUE(testUtils::areEqualSets(testEnum.getEnumValues(), testUtils::getTestEnumValues()));

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
}

TEST(EnumTest, registeredEnum) {
    babelwires::EnumRegistry enumReg;
    babelwires::Enum::EnumValues values = testUtils::getTestEnumValues();

    struct TestEnum : babelwires::RegisteredEnum<TestEnum> {
        TestEnum(babelwires::Enum::EnumValues& values)
            : RegisteredEnum<TestEnum>("TestEnum", "Test Enum", 1, values, 1) {}
    };

    EXPECT_EQ(TestEnum::getRegisteredInstance(), nullptr);
    enumReg.addEntry(std::make_unique<TestEnum>(values));
    EXPECT_NE(TestEnum::getRegisteredInstance(), nullptr);
}

#define TEST_ENUM_VALUES(X)                                                                                            \
    X(Foo, "Foo value", "00000000-1111-2222-3333-444444444444")                                                        \
    X(Bar, "Bar value", "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee")                                                        \
    X(Erm, "Erm value", "00000000-1111-2222-dddd-eeeeeeeeeeee")

ENUM_DEFINE_ENUM_VALUE_SOURCE(TEST_ENUM_VALUES);

TEST(EnumTest, enumWithCppEnum) {
    struct TestEnum : babelwires::RegisteredEnum<TestEnum> {
        TestEnum()
            : RegisteredEnum<TestEnum>("TestEnum", "Test Enum", 1, ENUM_IDENTIFIER_VECTOR(TEST_ENUM_VALUES), 1) {}
        
        ENUM_DEFINE_CPP_ENUM(TEST_ENUM_VALUES);
    };

    testUtils::TestLog log;
    babelwires::FieldNameRegistryScope fieldRegistry;
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
