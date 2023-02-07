/**
 * A very simple UUID class.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <Common/uuid.hpp>

#include <Common/exceptions.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iomanip>

namespace {
    const char zeroPattern[] = "00000000-0000-0000-0000-000000000000";
}

babelwires::Uuid::Uuid() = default;

babelwires::Uuid::Uuid(const std::string& uuidText) {
    assert(isValid(uuidText) && "The provided UUID was not in the correct form.");
    {
        std::stringstream tmp;
        tmp << uuidText.substr(0, 8) << uuidText.substr(9, 4) << uuidText.substr(14, 4);
        tmp >> std::hex >> m_high;
    }
    {
        std::stringstream tmp;
        tmp << uuidText.substr(19, 4) << uuidText.substr(24, 12);
        tmp >> std::hex >> m_low;
    }
}

bool babelwires::Uuid::isZero() const {
    return (m_low == 0) && (m_high == 0);
}

void babelwires::Uuid::randomize(std::default_random_engine& randomEngine) {
    do {
        m_high = std::uniform_int_distribution<std::uint64_t>()(randomEngine);
        m_low = std::uniform_int_distribution<std::uint64_t>()(randomEngine);
    } while (isZero());
}

babelwires::Uuid babelwires::Uuid::deserializeFromString(std::string_view uuidText) {
    if (!isValid(uuidText)) {
        throw ParseException() << "Failed to parse a UUID";
    }
    return Uuid(std::string(uuidText));
}

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
