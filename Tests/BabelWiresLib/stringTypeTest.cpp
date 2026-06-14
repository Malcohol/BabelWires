#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(StringTypeTest, stringTypeCreateValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::StringType stringType;

    babelwires::ValueHolder newValue = stringType.createValue(typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newStringValue = newValue->tryAs<babelwires::StringValue>();
    EXPECT_NE(newStringValue, nullptr);
    EXPECT_EQ(newStringValue->get(), babelwires::Text());
}

TEST(StringTypeTest, stringTypeIsValidValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::StringType stringType;

    babelwires::StringValue value(u8"Hello");

    EXPECT_TRUE(stringType.isValidValue(typeSystem, value));

    EXPECT_FALSE(stringType.isValidValue(typeSystem, babelwires::IntValue(5)));
    EXPECT_FALSE(stringType.isValidValue(typeSystem, babelwires::RationalValue(3)));
}

TEST(StringTypeTest, stringTypeGetKind) {
    testUtils::TestLog log;
    babelwires::StringType stringType;

    EXPECT_FALSE(stringType.getFlavour().empty());
}

TEST(StringTypeTest, stringTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::StringType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->tryAs<babelwires::StringType>(), nullptr);
}
