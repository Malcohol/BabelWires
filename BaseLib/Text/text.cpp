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
    constexpr const char c_nonUtf8Placeholder[] = "\uFFFD"; // Decode error replacement character

    bool isPrintableAsciiChar(char c) {
        // Printable ASCII characters are in the range 32-126 inclusive. 
        // 127 is DEL, which is not printable.
        return c >= 32 && c <= 126;
    }

    bool isPrintableAsciiString(std::string_view str) {
        for (const char c : str) {
            if (!isPrintableAsciiChar(c)) {
                return false;
            }
        }
        return true;
    }

    bool is7BitAsciiChar(char c) {
        // 7-bit ASCII characters are in the range 0-127 inclusive.
        return static_cast<unsigned char>(c) <= 127;
    }

    bool is7BitAsciiString(std::string_view str) {
        for (const char c : str) {
            if (!is7BitAsciiChar(c)) {
                return false;
            }
        }
        return true;
    }
}

babelwires::ResultT<babelwires::Text> babelwires::Text::fromPrintableAscii(std::string_view ascii) {
    if (!isPrintableAsciiString(ascii)) {
        return ResultT<Text>(ErrorStorage("Input string contains non-printable ASCII characters."));
    }
    return ResultT<Text>(Text(std::string(ascii)));
}

babelwires::ResultT<babelwires::Text> babelwires::Text::from7BitAscii(std::string_view ascii) {
    if (!is7BitAsciiString(ascii)) {
        return ResultT<Text>(ErrorStorage("Input string contains non-7-bit ASCII characters."));
    }
    return ResultT<Text>(Text(std::string(ascii)));
}

babelwires::ResultT<babelwires::Text> babelwires::Text::fromUtf8(std::string_view utf8) {
    // TODO Validate UTF-8 encoding. For now, we're just getting the API right, so assume the input is valid UTF-8.
    return ResultT<Text>(Text(std::string(utf8)));
}

babelwires::Text babelwires::Text::assertFromPrintableAscii(std::string_view ascii) {
    assert(isPrintableAsciiString(ascii) && "Input string contains non-printable ASCII characters.");
    return Text(std::string(ascii));
}

babelwires::Text babelwires::Text::assertFrom7BitAscii(std::string_view ascii) {
    assert(is7BitAsciiString(ascii) && "Input string contains non-7-bit ASCII characters.");
    return Text(std::string(ascii));
}

babelwires::Text babelwires::Text::assertFromUtf8(std::string_view utf8) {
    // TODO Validate UTF-8 encoding. For now, we're just getting the API right, so assume the input is valid UTF-8.
    return Text(std::string(utf8));
}

babelwires::Text babelwires::Text::tryFromPrintableAscii(std::string_view ascii) {
    // This is a lossy conversion, so we will replace non-printable ASCII characters with a placeholder.
    std::string result;
    result.reserve(ascii.size());
    for (const char c : ascii) {
        if (isPrintableAsciiChar(c)) {
            result.push_back(c);
        } else {
            result.push_back(c_nonPrintablePlaceholder);
        }
    }
    return Text(std::move(result));
}

babelwires::Text babelwires::Text::tryFrom7BitAscii(std::string_view ascii) {
    // This is a lossy conversion, so we will replace non-7-bit ASCII characters with a placeholder.
    std::string result;
    result.reserve(ascii.size());
    for (const char c : ascii) {
        if (is7BitAsciiChar(c)) {
            result.push_back(c);
        } else {
            result.push_back(c_non7BitPlaceholder);
        }
    }
    return Text(std::move(result));
}

babelwires::Text babelwires::Text::tryFromUtf8(std::string_view utf8) {
    // This is a lossy conversion, so we will replace invalid UTF-8 sequences with a placeholder.
    // TODO Implement proper UTF-8 validation and replacement. For now, we're just getting the API right, so assume the input is valid UTF-8.
    return Text(std::string(utf8));
}

bool babelwires::Text::isPrintableAscii() const {
    return isPrintableAsciiString(m_data);
}

bool babelwires::Text::is7BitAscii() const {
    return is7BitAsciiString(m_data);
}

babelwires::ResultT<std::string> babelwires::Text::toPrintableAscii() const {
    if (!isPrintableAscii()) {
        return ResultT<std::string>(ErrorStorage("Stored text contains non-printable ASCII characters."));
    }
    return ResultT<std::string>(m_data);
}

babelwires::ResultT<std::string> babelwires::Text::to7BitAscii() const {
    if (!is7BitAscii()) {
        return ResultT<std::string>(ErrorStorage("Stored text contains non-7-bit ASCII characters."));
    }
    return ResultT<std::string>(m_data);
}

std::string babelwires::Text::assertToPrintableAscii() const {
    assert(isPrintableAscii() && "Stored text contains non-printable ASCII characters.");
    return m_data;
}

std::string babelwires::Text::assertTo7BitAscii() const {
    assert(is7BitAscii() && "Stored text contains non-7-bit ASCII characters.");
    return m_data;
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

std::string_view babelwires::Text::toUtf8() const {
    // This always succeeds, as the stored data is always in UTF-8 encoding.
    return m_data;
}
