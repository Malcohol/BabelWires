#include <gtest/gtest.h>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Enum/enumWithCppEnum.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <Domains/TestDomain/testEnum.hpp>

#include <Tests/TestUtils/equalSets.hpp>

#include <Tests/TestUtils/testLog.hpp>

TEST(EnumTest, basic) {
    testUtils::TestLog log;
    testDomain::TestEnum testEnum;

    EXPECT_TRUE(testUtils::areEqualSets(testEnum.getValueSet(),
                                        babelwires::EnumType::ValueSet{"Foo", "Bar", "Erm", "Oom", "Boo"}));

    EXPECT_EQ(testEnum.getIndexOfDefaultValue(), 1);

    EXPECT_EQ(testEnum.getIndexFromIdentifier("Foo"), 0);
    EXPECT_EQ(testEnum.getIndexFromIdentifier("Bar"), 1);
    EXPECT_EQ(testEnum.getIndexFromIdentifier("Erm"), 2);
    EXPECT_EQ(testEnum.getIndexFromIdentifier("Oom"), 3);
    EXPECT_EQ(testEnum.getIndexFromIdentifier("Boo"), 4);
    EXPECT_EQ(testEnum.getIdentifierFromIndex(0), "Foo");
    EXPECT_EQ(testEnum.getIdentifierFromIndex(1), "Bar");
    EXPECT_EQ(testEnum.getIdentifierFromIndex(2), "Erm");
    EXPECT_EQ(testEnum.getIdentifierFromIndex(3), "Oom");
    EXPECT_EQ(testEnum.getIdentifierFromIndex(4), "Boo");

    EXPECT_TRUE(testEnum.isAValue("Foo"));
    EXPECT_TRUE(testEnum.isAValue("Bar"));
    EXPECT_TRUE(testEnum.isAValue("Erm"));
    EXPECT_TRUE(testEnum.isAValue("Oom"));
    EXPECT_TRUE(testEnum.isAValue("Boo"));

    EXPECT_FALSE(testEnum.isAValue("Flerm"));

    // An identifier with a non-zero discriminator that does not match
    babelwires::ShortId foo2("Foo");
    foo2.setDiscriminator(10000);
    EXPECT_FALSE(testEnum.isAValue(foo2));
}

TEST(EnumTest, enumWithCppEnum) {
    testUtils::TestLog log;
    testDomain::TestEnum testEnum;

    EXPECT_EQ(testEnum.getValueFromIdentifier("Foo"), testDomain::TestEnum::Value::Foo);
    EXPECT_EQ(testEnum.getValueFromIdentifier("Bar"), testDomain::TestEnum::Value::Bar);
    EXPECT_EQ(testEnum.getValueFromIdentifier("Erm"), testDomain::TestEnum::Value::Erm);
    EXPECT_EQ(testEnum.getValueFromIdentifier("Oom"), testDomain::TestEnum::Value::Oom);
    EXPECT_EQ(testEnum.getValueFromIdentifier("Boo"), testDomain::TestEnum::Value::Boo);
    EXPECT_EQ(testEnum.getIdentifierFromValue(testDomain::TestEnum::Value::Foo), "Foo");
    EXPECT_EQ(testEnum.getIdentifierFromValue(testDomain::TestEnum::Value::Bar), "Bar");
    EXPECT_EQ(testEnum.getIdentifierFromValue(testDomain::TestEnum::Value::Erm), "Erm");
    EXPECT_EQ(testEnum.getIdentifierFromValue(testDomain::TestEnum::Value::Oom), "Oom");
    EXPECT_EQ(testEnum.getIdentifierFromValue(testDomain::TestEnum::Value::Boo), "Boo");
}

TEST(EnumTest, createValue) {
    babelwires::TypeSystem typeSystem;
    testDomain::TestEnum testEnum;

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
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;

    typeSystem.addEntry<testDomain::TestEnum>();
    // Have to register Sub-Enums in the TypeSystem or they don't work.
    typeSystem.addEntry<testDomain::TestSubEnum>();

    const auto& testEnum = typeSystem.getRegisteredType<testDomain::TestEnum>();
    const auto& testSubEnum = typeSystem.getRegisteredType<testDomain::TestSubEnum>();

    auto [valueHolder, value] = testSubEnum->createValue(typeSystem);
    EXPECT_TRUE(valueHolder);
    auto enumValue = value.as<babelwires::EnumValue>();
    EXPECT_TRUE(enumValue);
    EXPECT_TRUE(testEnum->isValidValue(typeSystem, value));
    EXPECT_TRUE(testSubEnum->isValidValue(typeSystem, value));

    enumValue->set("Foo");
    EXPECT_TRUE(testEnum->isValidValue(typeSystem, value));
    EXPECT_FALSE(testSubEnum->isValidValue(typeSystem, value));

    enumValue->set("Flerm");
    EXPECT_FALSE(testEnum->isValidValue(typeSystem, value));
    EXPECT_FALSE(testSubEnum->isValidValue(typeSystem, value));
}