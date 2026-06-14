#include <gtest/gtest.h>

#include <BaseLib/Text/text.hpp>

namespace {
    // Note: It is important that this test does not depend on the source file encoding.
    // So it doesn't use `\uXXXX` escapes, which are interpreted according to the source file encoding.
    std::string makeString(std::initializer_list<char> chars) {
        return std::string(chars.begin(), chars.end());
    }

    const std::string& replacementCharacter() {
        static const std::string value = "\xEF\xBF\xBD";
        return value;
    }
} // namespace

TEST(TextTest, fromPrintableAsciiAcceptsPrintableTextButRejectsControlCharactersAndHighBitBytes) {
    const auto printableResult = babelwires::Text::fromPrintableAscii("Hello, world!");
    ASSERT_TRUE(printableResult);
    EXPECT_EQ(printableResult.value().toUtf8(), "Hello, world!");
    EXPECT_TRUE(printableResult.value().isPrintableAscii());
    EXPECT_TRUE(printableResult.value().is7BitAscii());

    const auto controlResult = babelwires::Text::fromPrintableAscii(makeString({'A', '\n', 'B'}));
    ASSERT_FALSE(controlResult);

    const auto highBitResult = babelwires::Text::fromPrintableAscii(makeString({'A', static_cast<char>(0x80), 'B'}));
    ASSERT_FALSE(highBitResult);
}

TEST(TextTest, from7BitAsciiAcceptsControlCharactersButRejectsHighBitBytes) {
    const auto controlResult = babelwires::Text::from7BitAscii(makeString({'A', '\n', static_cast<char>(0x7F), 'B'}));
    ASSERT_TRUE(controlResult);
    EXPECT_EQ(controlResult.value().toUtf8(), makeString({'A', '\n', static_cast<char>(0x7F), 'B'}));
    EXPECT_FALSE(controlResult.value().isPrintableAscii());
    EXPECT_TRUE(controlResult.value().is7BitAscii());

    const auto highBitResult = babelwires::Text::from7BitAscii(makeString({'A', static_cast<char>(0x80), 'B'}));
    ASSERT_FALSE(highBitResult);
}

TEST(TextTest, utf8EntryPointsRoundTripValidUtf8) {
    const std::string utf8 = "P\xC2\xA3";

    const auto result = babelwires::Text::fromUtf8(utf8);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value().toUtf8(), utf8);
    EXPECT_FALSE(result.value().isPrintableAscii());
    EXPECT_FALSE(result.value().is7BitAscii());

    const auto lossy = babelwires::Text::tryFromUtf8(utf8);
    EXPECT_EQ(lossy.toUtf8(), utf8);
}

TEST(TextTest, fromUtf8RejectsInvalidUtf8) {
    const auto strayContinuation = babelwires::Text::fromUtf8(makeString({'A', static_cast<char>(0x80), 'B'}));
    ASSERT_FALSE(strayContinuation);
    
    const auto overlong = babelwires::Text::fromUtf8(
        makeString({'A', static_cast<char>(0xE0), static_cast<char>(0x80), static_cast<char>(0x80), 'B'}));
    ASSERT_FALSE(overlong);
    
    const auto truncated =
        babelwires::Text::fromUtf8(makeString({'A', static_cast<char>(0xE2), static_cast<char>(0x82), 'B'}));
    ASSERT_FALSE(truncated);
}

TEST(TextTest, assertFromUtf8AssertsOnInvalidUtf8) {
    EXPECT_DEATH(babelwires::Text::assertFromUtf8(makeString({'A', static_cast<char>(0x80), 'B'})),
                 "Input string contains invalid UTF-8.");
}

TEST(TextTest, tryFromUtf8ReplacesInvalidSequencesWithReplacementCharacter) {
    const std::string invalidUtf8 =
        makeString({'A', static_cast<char>(0xE0), static_cast<char>(0x80), static_cast<char>(0x80), 'B',
                    static_cast<char>(0xE2), static_cast<char>(0x82), 'C', static_cast<char>(0x80), 'D'});

    const babelwires::Text text = babelwires::Text::tryFromUtf8(invalidUtf8);
    const std::string expected =
        std::string("A") + replacementCharacter() + "B" + replacementCharacter() + "C" + replacementCharacter() + "D";

    EXPECT_EQ(text.toUtf8(), expected);
    EXPECT_FALSE(text.isPrintableAscii());
    EXPECT_FALSE(text.is7BitAscii());
    EXPECT_TRUE(babelwires::Text::fromUtf8(text.toUtf8()));
}

TEST(TextTest, tryFromPrintableAsciiReplacesNonPrintableCharacters) {
    const babelwires::Text text = babelwires::Text::tryFromPrintableAscii(
        makeString({'A', '\n', 'B', static_cast<char>(0x7F), static_cast<char>(0x80), 'C'}));

    EXPECT_EQ(text.toUtf8(), "A?B??C");
    EXPECT_TRUE(text.isPrintableAscii());
    EXPECT_TRUE(text.is7BitAscii());
}

TEST(TextTest, tryFrom7BitAsciiPreservesControlsButReplacesHighBitBytes) {
    const babelwires::Text text =
        babelwires::Text::tryFrom7BitAscii(makeString({'A', '\n', static_cast<char>(0xC2), 'B'}));

    EXPECT_EQ(text.toUtf8(), makeString({'A', '\n', '?', 'B'}));
    EXPECT_FALSE(text.isPrintableAscii());
    EXPECT_TRUE(text.is7BitAscii());
}

TEST(TextTest, toPrintableAsciiSucceedsOnPrintableTextButRejectsControlCharactersAndHighBitBytes) {
    const babelwires::Text printable = babelwires::Text::assertFromPrintableAscii("ASCII");
    ASSERT_TRUE(printable.toPrintableAscii());
    EXPECT_EQ(printable.toPrintableAscii().value(), "ASCII");

    const babelwires::Text controls = babelwires::Text::assertFrom7BitAscii(makeString({'A', '\n', 'B'}));
    ASSERT_FALSE(controls.toPrintableAscii());

    const babelwires::Text utf8 = babelwires::Text::assertFromUtf8("P\xC2\xA3");
    ASSERT_FALSE(utf8.toPrintableAscii());
}

TEST(TextTest, to7BitAsciiSucceedsOnControlCharactersButRejectsHighBitBytes) {
    const babelwires::Text controls = babelwires::Text::assertFrom7BitAscii(makeString({'A', '\n', 'B'}));
    ASSERT_TRUE(controls.to7BitAscii());
    EXPECT_EQ(controls.to7BitAscii().value(), makeString({'A', '\n', 'B'}));

    const babelwires::Text utf8 = babelwires::Text::assertFromUtf8("P\xC2\xA3");
    ASSERT_FALSE(utf8.to7BitAscii());
}

TEST(TextTest, lossyAsciiOutputUsesQuestionMarkPlaceholders) {
    const babelwires::Text text =
        babelwires::Text::assertFromUtf8(makeString({'A', '\n', static_cast<char>(0xC2), static_cast<char>(0xA3), 'B'}));

    EXPECT_EQ(text.tryToPrintableAscii(), "A???B");
    EXPECT_EQ(text.tryTo7BitAscii(), makeString({'A', '\n', '?', '?', 'B'}));
}
