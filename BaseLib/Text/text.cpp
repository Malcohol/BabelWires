/**
 * The Text class stores a string in a canonical representation.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Text/text.hpp>

#include <cassert>

namespace {
    constexpr const char c_nonPrintablePlaceholder = '?';
    constexpr const char c_non7BitPlaceholder = '?';
    // Use explicit UTF-8 encoding for the replacement character to avoid any ambiguity with the source file encoding.
    constexpr const char8_t c_nonUtf8Placeholder[] = u8"\xEF\xBF\xBD";

    bool isPrintableAsciiChar(unsigned char c) {
        // Printable ASCII characters are in the range 32-126 inclusive.
        // 127 is DEL, which is not printable.
        return c >= 32 && c <= 126;
    }

    template<typename CHAR>
    bool isPrintableAsciiString(std::basic_string_view<CHAR> str) {
        for (const CHAR c : str) {
            if (!isPrintableAsciiChar(c)) {
                return false;
            }
        }
        return true;
    }

    bool is7BitAsciiChar(unsigned char c) {
        // 7-bit ASCII characters are in the range 0-127 inclusive.
        return static_cast<unsigned char>(c) <= 127;
    }

    template<typename CHAR>
    bool is7BitAsciiString(std::basic_string_view<CHAR> str) {
        for (const CHAR c : str) {
            if (!is7BitAsciiChar(c)) {
                return false;
            }
        }
        return true;
    }

    bool isUtf8ContinuationByte(unsigned char c) {
        return c >= 0x80 && c <= 0xBF;
    }

    std::size_t getUtf8SequenceLength(unsigned char leadByte) {
        if (leadByte <= 0x7F) {
            return 1;
        }
        if (leadByte >= 0xC2 && leadByte <= 0xDF) {
            return 2;
        }
        if (leadByte >= 0xE0 && leadByte <= 0xEF) {
            return 3;
        }
        if (leadByte >= 0xF0 && leadByte <= 0xF4) {
            return 4;
        }
        return 0;
    }

    template<typename CHAR>
    bool isValidUtf8Sequence(std::basic_string_view<CHAR> str, std::size_t index, std::size_t sequenceLength) {
        const unsigned char leadByte = static_cast<unsigned char>(str[index]);
        if (sequenceLength == 1) {
            return true;
        }

        const unsigned char secondByte = static_cast<unsigned char>(str[index + 1]);
        if (!isUtf8ContinuationByte(secondByte)) {
            return false;
        }

        if (sequenceLength == 2) {
            return true;
        }

        const unsigned char thirdByte = static_cast<unsigned char>(str[index + 2]);
        if (!isUtf8ContinuationByte(thirdByte)) {
            return false;
        }

        if (sequenceLength == 3) {
            // Check for overlong sequences and surrogates
            if (leadByte == 0xE0) {
                return secondByte >= 0xA0;
            }
            if (leadByte == 0xED) {
                return secondByte <= 0x9F;
            }
            return true;
        }

        const unsigned char fourthByte = static_cast<unsigned char>(str[index + 3]);
        if (!isUtf8ContinuationByte(fourthByte)) {
            return false;
        }

        // Check for overlong sequences and code points above U+10FFFF
        if (leadByte == 0xF0) {
            return secondByte >= 0x90;
        }
        if (leadByte == 0xF4) {
            return secondByte <= 0x8F;
        }
        return true;
    }

    template<typename CHAR>
    bool isValidUtf8String(std::basic_string_view<CHAR> str) {
        std::size_t index = 0;
        while (index < str.size()) {
            const std::size_t sequenceLength = getUtf8SequenceLength(static_cast<unsigned char>(str[index]));
            if (sequenceLength == 0 || index + sequenceLength > str.size()) {
                return false;
            }
            if (!isValidUtf8Sequence(str, index, sequenceLength)) {
                return false;
            }
            index += sequenceLength;
        }
        return true;
    }

    std::size_t consumeInvalidUtf8Sequence(std::string_view str, std::size_t index, std::size_t sequenceLength) {
        if (sequenceLength == 0) {
            return index + 1;
        }

        std::size_t nextIndex = index + 1;
        while (nextIndex < str.size() && (nextIndex - index) < sequenceLength &&
               isUtf8ContinuationByte(static_cast<unsigned char>(str[nextIndex]))) {
            ++nextIndex;
        }
        return nextIndex;
    }

    std::u8string replaceInvalidUtf8(std::string_view utf8) {
        std::u8string result;
        result.reserve(utf8.size());

        std::size_t index = 0;
        while (index < utf8.size()) {
            const std::size_t sequenceLength = getUtf8SequenceLength(static_cast<unsigned char>(utf8[index]));
            if ((sequenceLength != 0) && ((index + sequenceLength) <= utf8.size()) &&
                isValidUtf8Sequence(utf8, index, sequenceLength)) {
                for (std::size_t i = 0; i < sequenceLength; ++i) {
                    result.push_back(static_cast<char8_t>(utf8[index + i]));
                }
                index += sequenceLength;
            } else {
                result.append(c_nonUtf8Placeholder);
                index = consumeInvalidUtf8Sequence(utf8, index, sequenceLength);
            }
        }

        return result;
    }
} // namespace

babelwires::Text::Text(std::u8string data)
    : m_data(std::move(data)) {
        assert(isValidUtf8String(std::u8string_view(m_data)) && "Input string contains invalid UTF-8.");
    }

babelwires::ResultT<babelwires::Text> babelwires::Text::fromPrintableAscii(std::string_view ascii) {
    if (!isPrintableAsciiString(ascii)) {
        return ResultT<Text>(ErrorStorage("Input string contains non-printable ASCII characters."));
    }
    return ResultT<Text>(Text(ascii));
}

babelwires::ResultT<babelwires::Text> babelwires::Text::from7BitAscii(std::string_view ascii) {
    if (!is7BitAsciiString(ascii)) {
        return ResultT<Text>(ErrorStorage("Input string contains non-7-bit ASCII characters."));
    }
    return ResultT<Text>(Text(ascii));
}

babelwires::ResultT<babelwires::Text> babelwires::Text::fromUtf8(std::string_view utf8) {
    if (!isValidUtf8String(utf8)) {
        return ResultT<Text>(ErrorStorage("Input string contains invalid UTF-8."));
    }
    return ResultT<Text>(Text(utf8));
}

babelwires::Text babelwires::Text::assertFromPrintableAscii(std::string_view ascii) {
    assert(isPrintableAsciiString(ascii) && "Input string contains non-printable ASCII characters.");
    return Text(ascii);
}

babelwires::Text babelwires::Text::assertFrom7BitAscii(std::string_view ascii) {
    assert(is7BitAsciiString(ascii) && "Input string contains non-7-bit ASCII characters.");
    return Text(ascii);
}

babelwires::Text babelwires::Text::assertFromUtf8(std::string_view utf8) {
    assert(isValidUtf8String(utf8) && "Input string contains invalid UTF-8.");
    return Text(utf8);
}

babelwires::Text babelwires::Text::tryFromPrintableAscii(std::string_view ascii) {
    // This is a lossy conversion, so we will replace non-printable ASCII characters with a placeholder.
    std::u8string result;
    result.reserve(ascii.size());
    for (const char c : ascii) {
        if (isPrintableAsciiChar(c)) {
            result.push_back(c);
        } else {
            result.push_back(c_nonPrintablePlaceholder);
        }
    }
    return Text(result);
}

babelwires::Text babelwires::Text::tryFrom7BitAscii(std::string_view ascii) {
    // This is a lossy conversion, so we will replace non-7-bit ASCII characters with a placeholder.
    std::u8string result;
    result.reserve(ascii.size());
    for (const char c : ascii) {
        if (is7BitAsciiChar(c)) {
            result.push_back(c);
        } else {
            result.push_back(c_non7BitPlaceholder);
        }
    }
    return Text(result);
}

babelwires::Text babelwires::Text::tryFromUtf8(std::string_view utf8) {
    // This is a lossy conversion, so we will replace invalid UTF-8 sequences with a placeholder.
    return Text(replaceInvalidUtf8(utf8), true);
}

bool babelwires::Text::isPrintableAscii() const {
    return isPrintableAsciiString(std::u8string_view(m_data));
}

bool babelwires::Text::is7BitAscii() const {
    return is7BitAsciiString(std::u8string_view(m_data));
}

babelwires::ResultT<std::string> babelwires::Text::toPrintableAscii() const {
    if (!isPrintableAscii()) {
        return ResultT<std::string>(ErrorStorage("Stored text contains non-printable ASCII characters."));
    }
    return ResultT<std::string>(std::string(m_data.begin(), m_data.end()));
}

babelwires::ResultT<std::string> babelwires::Text::to7BitAscii() const {
    if (!is7BitAscii()) {
        return ResultT<std::string>(ErrorStorage("Stored text contains non-7-bit ASCII characters."));
    }
    return ResultT<std::string>(std::string(m_data.begin(), m_data.end()));
}

std::string babelwires::Text::assertToPrintableAscii() const {
    assert(isPrintableAscii() && "Stored text contains non-printable ASCII characters.");
    return std::string(m_data.begin(), m_data.end());
}

std::string babelwires::Text::assertTo7BitAscii() const {
    assert(is7BitAscii() && "Stored text contains non-7-bit ASCII characters.");
    return std::string(m_data.begin(), m_data.end());
}

std::string babelwires::Text::tryToPrintableAscii() const {
    // This is a lossy conversion, so we will replace non-printable ASCII characters with a placeholder.
    std::string result;
    result.reserve(m_data.size());
    for (const char c : m_data) {
        if (isPrintableAsciiChar(c)) {
            result.push_back(c);
        } else {
            result.push_back(c_nonPrintablePlaceholder);
        }
    }
    return result;
}

std::string babelwires::Text::tryTo7BitAscii() const {
    // This is a lossy conversion, so we will replace non-7-bit ASCII characters with a placeholder.
    std::string result;
    result.reserve(m_data.size());
    for (const char c : m_data) {
        if (is7BitAsciiChar(c)) {
            result.push_back(c);
        } else {
            result.push_back(c_non7BitPlaceholder);
        }
    }
    return result;
}

std::string babelwires::Text::toUtf8() const {
    // This always succeeds, as the stored data is always in UTF-8 encoding.
    return std::string(m_data.begin(), m_data.end());
}

std::string babelwires::Text::serializeToString() const {
    return toUtf8();
}

babelwires::ResultT<babelwires::Text> babelwires::Text::deserializeFromString(std::string_view str) {
    return Text::fromUtf8(str);
}
