#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Text/textType.hpp>
#include <BabelWiresLib/Types/Text/textTypeConstructor.hpp>
#include <BabelWiresLib/Types/Text/textValue.hpp>

#include <Tests/BabelWiresLib/TestUtils/testEnvironment.hpp>

TEST(TextTypeTest, defaultTextTypeCreateValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultTextType defaultTextType;

    babelwires::ValueHolder newValue = defaultTextType.createValue(typeSystem);
    EXPECT_TRUE(newValue);

    const auto* const newStringValue = newValue->tryAs<babelwires::TextValue>();
    EXPECT_NE(newStringValue, nullptr);
    EXPECT_EQ(newStringValue->get(), babelwires::Text());
}

TEST(TextTypeTest, defaultTextTypeIsValidValue) {
    testUtils::TestLog log;
    babelwires::TypeSystem typeSystem;
    babelwires::DefaultTextType defaultTextType;

    babelwires::TextValue value(u8"Hello");

    EXPECT_TRUE(defaultTextType.isValidValue(typeSystem, value));

    EXPECT_FALSE(defaultTextType.isValidValue(typeSystem, babelwires::IntValue(5)));
}

TEST(TextTypeTest, constructedTextType) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp textTypeExp(babelwires::TextTypeConstructor::getThisIdentifier(), babelwires::IntValue(5));

    babelwires::TypePtr type = textTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type->tryAs<babelwires::TextType>(), nullptr);
    EXPECT_EQ(type->tryAs<babelwires::TextType>()->getMaxLength(), 5);

    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue()));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"")));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"H")));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"He")));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"Hel")));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"Hell")));
    EXPECT_TRUE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"Hello")));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::TextValue(u8"Hello!")));
    EXPECT_FALSE(type->isValidValue(testEnvironment.m_typeSystem, babelwires::IntValue(5)));
}

TEST(TextTypeTest, makeTypeExp) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp textTypeExp = babelwires::TextTypeConstructor::makeTypeExp(5);

    babelwires::TypePtr type = textTypeExp.tryResolve(testEnvironment.m_typeSystem);

    EXPECT_NE(type->tryAs<babelwires::TextType>(), nullptr);
    EXPECT_EQ(type->tryAs<babelwires::TextType>()->getMaxLength(), 5);
}

TEST(TextTypeTest, textTypeGetKind) {
    testUtils::TestLog log;
    babelwires::DefaultTextType defaultTextType;

    EXPECT_FALSE(defaultTextType.getFlavour().empty());
}

TEST(TextTypeTest, textTypeAndConstructorAreRegistered) {
    testUtils::TestEnvironment testEnvironment;

    const babelwires::TypePtr foundType =
        testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::DefaultTextType::getThisIdentifier());
    EXPECT_TRUE(foundType);
    EXPECT_NE(foundType->tryAs<babelwires::DefaultTextType>(), nullptr);

    const babelwires::TypeConstructor* foundConstructor =
        testEnvironment.m_typeSystem.tryGetTypeConstructorById(babelwires::TextTypeConstructor::getThisIdentifier());
    EXPECT_TRUE(foundConstructor);
    EXPECT_NE(foundConstructor->tryAs<babelwires::TextTypeConstructor>(), nullptr);
}

TEST(TextTypeTest, typeName) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp textTypeExp(babelwires::TextTypeConstructor::getThisIdentifier(), babelwires::IntValue(18));

    EXPECT_EQ(textTypeExp.toString(), "Text[18]");
}

TEST(TextTypeTest, subtype) {
    testUtils::TestEnvironment testEnvironment;

    babelwires::TypeExp textTypeExp5(babelwires::TextTypeConstructor::getThisIdentifier(), babelwires::IntValue(5));
    babelwires::TypeExp textTypeExp10(babelwires::TextTypeConstructor::getThisIdentifier(), babelwires::IntValue(10));

    const babelwires::TypePtr textType5 = textTypeExp5.assertResolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr textType10 = textTypeExp10.assertResolve(testEnvironment.m_typeSystem);
    const babelwires::TypePtr defaultTextType = testEnvironment.m_typeSystem.tryGetRegisteredTypeById(babelwires::DefaultTextType::getThisIdentifier());

    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*textType5, *textType5), babelwires::SubtypeOrder::IsEquivalent);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*textType5, *textType10), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*textType10, *textType5), babelwires::SubtypeOrder::IsSupertype);
    
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*textType5, *defaultTextType), babelwires::SubtypeOrder::IsSubtype);
    EXPECT_EQ(testEnvironment.m_typeSystem.compareSubtype(*defaultTextType, *textType5), babelwires::SubtypeOrder::IsSupertype);
}
