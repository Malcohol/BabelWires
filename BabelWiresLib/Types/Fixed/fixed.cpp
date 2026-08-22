/**
 * Fixed point value implementation.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Fixed/fixed.hpp>

#include <BaseLib/Result/error.hpp>
#include <BaseLib/Result/resultDSL.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ostream>
#include <sstream>

namespace {
    constexpr bool isDigit(char c) {
        return (c >= '0') && (c <= '9');
    }

    std::uint64_t parseUnsigned(std::string_view str) {
        std::uint64_t value = 0;
        for (char c : str) {
            value = value * 10 + static_cast<std::uint64_t>(c - '0');
        }
        return value;
    }
} // namespace

babelwires::Fixed::Fixed() = default;

babelwires::Fixed::Fixed(NativeType numerator, int precision)
    : m_numerator(normalizeNumerator(numerator))
    , m_precision(normalizePrecision(precision)) {
    assert(m_precision >= 0);
    if (m_precision > s_maxPrecision) {
        m_precision = s_maxPrecision;
    }
}

babelwires::Fixed::NativeType babelwires::Fixed::getNumerator() const {
    return m_numerator;
}

int babelwires::Fixed::getPrecision() const {
    return m_precision;
}

void babelwires::Fixed::setNumerator(NativeType numerator) {
    m_numerator = normalizeNumerator(numerator);
}

void babelwires::Fixed::setPrecision(int precision) {
    m_precision = normalizePrecision(precision);
    assert(m_precision >= 0);
}

bool babelwires::Fixed::operator==(const Fixed& other) const {
    return (m_precision == other.m_precision) && (m_numerator == other.m_numerator);
}

bool babelwires::Fixed::operator!=(const Fixed& other) const {
    return !(*this == other);
}

bool babelwires::Fixed::operator<(const Fixed& other) const {
    assert(m_precision == other.m_precision);
    return m_numerator < other.m_numerator;
}

bool babelwires::Fixed::operator>(const Fixed& other) const {
    assert(m_precision == other.m_precision);
    return m_numerator > other.m_numerator;
}

bool babelwires::Fixed::operator<=(const Fixed& other) const {
    assert(m_precision == other.m_precision);
    return m_numerator <= other.m_numerator;
}

bool babelwires::Fixed::operator>=(const Fixed& other) const {
    assert(m_precision == other.m_precision);
    return m_numerator >= other.m_numerator;
}

std::string babelwires::Fixed::toString() const {
    if (m_precision == 0) {
        return std::to_string(m_numerator);
    }
    const std::uint64_t factor = pow10(m_precision);
    const std::uint64_t magnitude = abs64(m_numerator);
    const std::uint64_t whole = magnitude / factor;
    const std::uint64_t fractional = magnitude % factor;

    std::string fractionText = std::to_string(fractional);
    const std::size_t requiredDigits = static_cast<std::size_t>(m_precision);
    if (fractionText.size() < requiredDigits) {
        fractionText.insert(0, requiredDigits - fractionText.size(), '0');
    }

    std::ostringstream os;
    if (m_numerator < 0) {
        os << '-';
    }
    os << whole << '.' << fractionText;
    return os.str();
}

babelwires::ResultT<babelwires::Fixed> babelwires::Fixed::deserializeFromString(std::string_view str) {
    if (str.empty()) {
        return Error() << "Fixed::deserializeFromString expected a value but got an empty string.";
    }

    std::size_t pos = 0;
    bool isNegative = false;
    if ((str[pos] == '+') || (str[pos] == '-')) {
        isNegative = (str[pos] == '-');
        ++pos;
    }
    if (pos == str.size()) {
        return Error() << "Fixed::deserializeFromString has no digits.";
    }

    const std::size_t decimalPos = str.find('.', pos);
    if (decimalPos != std::string_view::npos) {
        if (str.find('.', decimalPos + 1) != std::string_view::npos) {
            return Error() << "Fixed::deserializeFromString contains multiple decimal points.";
        }
        const std::string_view integerPart = str.substr(pos, decimalPos - pos);
        const std::string_view fractionalPart = str.substr(decimalPos + 1);
        if (integerPart.empty() || fractionalPart.empty()) {
            return Error() << "Fixed::deserializeFromString requires digits on both sides of the decimal point.";
        }
        if (!std::all_of(integerPart.begin(), integerPart.end(), isDigit) ||
            !std::all_of(fractionalPart.begin(), fractionalPart.end(), isDigit)) {
            return Error() << "Fixed::deserializeFromString contains non-digit characters.";
        }

        const int precision = static_cast<int>(fractionalPart.size());
        if (precision > s_maxPrecision) {
            return Error() << "Fixed precision exceeds the maximum supported value of " << s_maxPrecision;
        }

        const std::uint64_t factor = pow10(precision);
        const std::uint64_t integerValue = parseUnsigned(integerPart);
        const std::uint64_t fractionalValue = parseUnsigned(fractionalPart);
        const std::uint64_t combined = integerValue * factor + fractionalValue;
        return Fixed(static_cast<NativeType>((isNegative ? -1 : 1) * static_cast<NativeType>(combined)), precision);
    }

    const std::string_view integerPart = str.substr(pos);
    if (integerPart.empty() || !std::all_of(integerPart.begin(), integerPart.end(), isDigit)) {
        return Error() << "Fixed::deserializeFromString contains invalid integer digits.";
    }

    const std::uint64_t integerValue = parseUnsigned(integerPart);
    return Fixed(static_cast<NativeType>((isNegative ? -1 : 1) * static_cast<NativeType>(integerValue)), 0);
}

std::size_t babelwires::Fixed::getHash() const {
    return hash::mixtureOf(0x14A6, m_numerator, m_precision);
}

babelwires::Fixed::NativeType babelwires::Fixed::normalizeNumerator(NativeType numerator) {
    if (numerator == 0) {
        return 0;
    }
    return numerator;
}

int babelwires::Fixed::normalizePrecision(int precision) {
    if (precision < 0) {
        return 0;
    }
    return std::min(precision, s_maxPrecision);
}

std::uint64_t babelwires::Fixed::pow10(int precision) {
    std::uint64_t value = 1;
    for (int i = 0; i < precision; ++i) {
        value *= 10;
    }
    return value;
}

std::uint64_t babelwires::Fixed::abs64(NativeType value) {
    if (value >= 0) {
        return static_cast<std::uint64_t>(value);
    }
    return static_cast<std::uint64_t>(-(value + 1)) + 1;
}
