#include "Common/uuid.hpp"

#include "Common/exceptions.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>

babelwires::Uuid::Uuid(std::string uuidText)
    : m_text(std::move(uuidText)) {
    assert(isValid(m_text) && "The provided UUID was not in the correct form.");
    // It's not required of the input, but apparently, UUIDs should be written in lower case digits.
    std::transform(m_text.begin(), m_text.end(), m_text.begin(), [](unsigned char c) { return std::tolower(c); });
}

babelwires::Uuid babelwires::Uuid::deserializeFromString(std::string_view uuidText) {
    if (!isValid(uuidText)) {
        throw ParseException() << "Failed to parse a UUID.";
    }
    return Uuid(std::string(uuidText));
}

// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx

bool babelwires::Uuid::isValid(std::string_view uuidText) {
    constexpr unsigned char hyphenIndices[] = {8, 13, 18, 23, 255};
    constexpr int numHyphens = sizeof(hyphenIndices) - 1;
    constexpr int requiredNumDigits = 32 + numHyphens;

    if (uuidText.size() != requiredNumDigits) {
        return false;
    }
    int hyphenIndicesIndex = 0;
    for (unsigned char i = 0; i < requiredNumDigits; ++i) {
        if (i == hyphenIndices[hyphenIndicesIndex]) {
            if (uuidText[i] != '-') {
                return false;
            }
            ++hyphenIndicesIndex;
        } else if (!std::isxdigit(uuidText[i])) {
            return false;
        }
    }
    return true;
}
