#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(StringTypeTest, stringTypeCreateValue) {
    babelwires::TypeSystem typeSystem;
    babelwires::StringType stringType;

    babelwires::ValueHolder newValue = stringType.createValue(typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newStringValue = newValue->as<babelwires::StringValue>();
    EXPECT_NE(newStringValue, nullptr);
    EXPECT_EQ(newStringValue->get(), std::string());
}

TEST(StringTypeTest, stringTypeIsValidValue) {
    babelwires::TypeSystem typeSystem;
    babelwires::StringType stringType;

    babelwires::StringValue value("Hello");

    EXPECT_TRUE(stringType.isValidValue(typeSystem, value));

    EXPECT_FALSE(stringType.isValidValue(typeSystem, babelwires::IntValue(5)));
    EXPECT_FALSE(stringType.isValidValue(typeSystem, babelwires::RationalValue(3)));
}

TEST(StringTypeTest, stringTypeGetKind) {
    babelwires::StringType stringType;

    EXPECT_FALSE(stringType.getFlavour().empty());
}

TEST(StringTypeTest, stringTypeIsRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::StringType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->as<babelwires::StringType>(), nullptr);
}
