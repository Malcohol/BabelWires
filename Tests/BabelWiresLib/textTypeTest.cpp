#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(TextTypeTest, textTypeCreateValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultTextType defaultTextType;

    babelwires::ValueHolder newValue = defaultTextType.createValue(typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newStringValue = newValue->tryAs<babelwires::TextValue>();
    EXPECT_NE(newStringValue, nullptr);
    EXPECT_EQ(newStringValue->get(), babelwires::Text());
}

TEST(TextTypeTest, textTypeIsValidValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultTextType defaultTextType;

    babelwires::TextValue value(u8"Hello");

    EXPECT_TRUE(defaultTextType.isValidValue(typeSystem, value));

    EXPECT_FALSE(defaultTextType.isValidValue(typeSystem, babelwires::IntValue(5)));
    EXPECT_FALSE(defaultTextType.isValidValue(typeSystem, babelwires::RationalValue(3)));
}

TEST(TextTypeTest, textTypeGetKind) {
    testUtils::TestLog log;
    babelwires::DefaultTextType defaultTextType;

    EXPECT_FALSE(defaultTextType.getFlavour().empty());
}

TEST(TextTypeTest, textTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::DefaultTextType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->tryAs<babelwires::DefaultTextType>(), nullptr);
}
