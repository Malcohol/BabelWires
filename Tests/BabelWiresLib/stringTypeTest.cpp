#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(StringTypeTest, stringTypeCreateValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::TextType textType;

    babelwires::ValueHolder newValue = textType.createValue(typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newStringValue = newValue->tryAs<babelwires::TextValue>();
    EXPECT_NE(newStringValue, nullptr);
    EXPECT_EQ(newStringValue->get(), babelwires::Text());
}

TEST(StringTypeTest, stringTypeIsValidValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::TextType textType;

    babelwires::TextValue value(u8"Hello");

    EXPECT_TRUE(textType.isValidValue(typeSystem, value));

    EXPECT_FALSE(textType.isValidValue(typeSystem, babelwires::IntValue(5)));
    EXPECT_FALSE(textType.isValidValue(typeSystem, babelwires::RationalValue(3)));
}

TEST(StringTypeTest, stringTypeGetKind) {
    testUtils::TestLog log;
    babelwires::TextType textType;

    EXPECT_FALSE(textType.getFlavour().empty());
}

TEST(StringTypeTest, stringTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::TextType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->tryAs<babelwires::TextType>(), nullptr);
}
