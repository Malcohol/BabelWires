/**
 * Fixed point value implementation.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Math/fixed.hpp>

#include <BaseLib/Result/error.hpp>
#include <BaseLib/Result/resultDSL.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ostream>
#include <sstream>
#include <iomanip>

namespace {
    constexpr bool isDigit(char c) {
        return (c >= '0') && (c <= '9');
    }

    std::uint64_t pow10(int precision) {
        std::uint64_t value = 1;
        for (int i = 0; i < precision; ++i) {
            value *= 10;
        }
        return value;
    }

    std::uint64_t abs64(babelwires::Fixed::NativeType value) {
        if (value >= 0) {
            return static_cast<std::uint64_t>(value);
        }
        return static_cast<std::uint64_t>(-(value + 1)) + 1;
    }

    std::string toStringHelper(babelwires::Fixed::NativeType numerator, int precision, int minDisplayPrecision) {
        assert(precision >= 0);
        assert(minDisplayPrecision >= 0);
        assert(minDisplayPrecision <= precision);
        std::ostringstream oss;
        if (precision == 0) {
            oss << numerator;
        } else {
            const std::uint64_t factor = pow10(precision);
            const std::uint64_t magnitude = abs64(numerator);
            const std::uint64_t integerPart = magnitude / factor;
            const std::uint64_t fractionalPart = magnitude % factor;
            if (numerator < 0) {
                oss << '-';
            }
            oss << integerPart;
            if ((minDisplayPrecision > 0) || (fractionalPart != 0)) {
                // Ensure the fractional part has the required number of leading zeroes.
                const std::string fractionalPartStr = std::to_string(factor + fractionalPart);
                std::string_view fractionalPartView = fractionalPartStr.c_str() + 1;
                // Trim trailing zeros if the fractional part is longer than the minimum display precision
                while (fractionalPartView.length() > static_cast<std::size_t>(minDisplayPrecision) && fractionalPartView.back() == '0') {
                    fractionalPartView.remove_suffix(1);
                }
                oss << '.' << fractionalPartView;
            }
        }
        return oss.str();
    }

} // namespace

babelwires::Fixed::Fixed() = default;

babelwires::Fixed::Fixed(NativeType numerator, int precision)
    : m_numerator(numerator)
    , m_precision(precision) {
    assert(precision >= 0);
    assert(precision <= s_maxPrecision);
}

babelwires::ResultT<babelwires::Fixed> babelwires::Fixed::fromDouble(double value, int precision) {
    assert(precision >= 0);
    assert(precision <= s_maxPrecision);

    if (!std::isfinite(value)) {
        return Error() << "Cannot convert non-finite double to Fixed";
    }

    const double scaledValue = std::round(value * static_cast<double>(pow10(precision)));
    if (scaledValue > static_cast<double>(std::numeric_limits<NativeType>::max())
        || scaledValue < static_cast<double>(std::numeric_limits<NativeType>::min())) {
        return Error() << "Fixed value " << value << " overflows when converted to precision " << precision;
    }
    return Fixed(static_cast<NativeType>(scaledValue), precision);
}

babelwires::Fixed babelwires::Fixed::tryFromDouble(double value, int precision) {
    assert(precision >= 0);
    assert(precision <= s_maxPrecision);

    if (!std::isfinite(value)) {
        return Fixed(0, precision);
    }

    const double scaledValue = std::round(value * static_cast<double>(pow10(precision)));
    if (scaledValue > static_cast<double>(std::numeric_limits<NativeType>::max())) {
        return Fixed(std::numeric_limits<NativeType>::max(), precision);
    }
    if (scaledValue < static_cast<double>(std::numeric_limits<NativeType>::min())) {
        return Fixed(std::numeric_limits<NativeType>::min(), precision);
    }
    return Fixed(static_cast<NativeType>(scaledValue), precision);
}

babelwires::Fixed babelwires::Fixed::assertFromDouble(double value, int precision) {
    return ASSERT_NO_ERROR(fromDouble(value, precision));
}

babelwires::ResultT<babelwires::Fixed> babelwires::Fixed::fromFixed(Fixed value, int precision) {
    assert(precision >= 0);
    assert(precision <= s_maxPrecision);

    if (value.m_precision == precision) {
        return value;
    } else if (value.m_precision < precision) {
        // Handle overflow.
        const auto scale = static_cast<std::uint64_t>(pow10(precision - value.m_precision));
        const bool isNegative = value.m_numerator < 0;
        const std::uint64_t magnitude = abs64(value.m_numerator);
        if (magnitude > (std::numeric_limits<NativeType>::max() / scale)) {
            return Error() << "Fixed value " << value.toString() << " overflows when converted to precision "
                           << precision;
        }
        const NativeType scaledNumerator = static_cast<NativeType>(magnitude) * static_cast<NativeType>(scale);
        return Fixed(isNegative ? -scaledNumerator : scaledNumerator, precision);
    } else {
        const NativeType factor = static_cast<NativeType>(pow10(value.m_precision - precision));
        if ((value.m_numerator % factor) == 0) {
            return Fixed(value.m_numerator / factor, precision);
        }
        return Error() << "Fixed value " << value.toString() << " cannot be represented at precision " << precision;
    }
}

babelwires::Fixed babelwires::Fixed::tryFromFixed(Fixed value, int precision) {
    assert(precision >= 0);
    assert(precision <= s_maxPrecision);

    if (value.m_precision == precision) {
        return value;
    } else if (value.m_precision < precision) {
        // Handle overflow.
        const auto scale = static_cast<std::uint64_t>(pow10(precision - value.m_precision));
        const bool isNegative = value.m_numerator < 0;
        const std::uint64_t magnitude = abs64(value.m_numerator);
        if (magnitude > (std::numeric_limits<NativeType>::max() / scale)) {
            return Fixed(isNegative ? std::numeric_limits<NativeType>::min() : std::numeric_limits<NativeType>::max(),
                         precision);
        }
        const NativeType scaledNumerator = static_cast<NativeType>(magnitude) * static_cast<NativeType>(scale);
        return Fixed(isNegative ? -scaledNumerator : scaledNumerator, precision);
    } else {
        const NativeType factor = static_cast<NativeType>(pow10(value.m_precision - precision));
        const NativeType magnitude = std::llabs(value.m_numerator);
        const NativeType quotient = magnitude / factor;
        const NativeType remainder = magnitude % factor;
        NativeType roundedMagnitude = quotient;
        if ((remainder * 2) >= factor) {
            ++roundedMagnitude;
        }
        if (value.m_numerator < 0) {
            roundedMagnitude = -roundedMagnitude;
        }
        return Fixed(roundedMagnitude, precision);
    }
}

babelwires::Fixed babelwires::Fixed::assertFromFixed(Fixed value, int precision) {
    return ASSERT_NO_ERROR(fromFixed(value, precision));
}

babelwires::Fixed::NativeType babelwires::Fixed::getNumerator() const {
    return m_numerator;
}

int babelwires::Fixed::getPrecision() const {
    return m_precision;
}

void babelwires::Fixed::setNumerator(NativeType numerator) {
    m_numerator = numerator;
}

void babelwires::Fixed::setPrecision(int precision) {
    assert(precision >= 0);
    assert(precision <= s_maxPrecision);
    m_precision = precision;
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
    // For display, only show the necessary precision.
    return toStringHelper(m_numerator, m_precision, 0);
}

std::string babelwires::Fixed::serializeToString() const {
    // For serialization, we always use the full precision to ensure exact reconstruction.
    return toStringHelper(m_numerator, m_precision, m_precision);
}

double babelwires::Fixed::toDouble() const {
    return static_cast<double>(m_numerator) / static_cast<double>(pow10(m_precision));
}

babelwires::ResultT<babelwires::Fixed> babelwires::Fixed::deserializeFromString(std::string_view str) {
    Fixed fixedValue;
    const PartialParseResult parseResult = partialParse(str, fixedValue);
    if (parseResult == PartialParseResult::Success) {
        return fixedValue;
    } else if (parseResult == PartialParseResult::Failure) {
        return Error() << "Failed to parse Fixed value from string: " << str;
    } else {
        return Error() << "Failed to parse Fixed value from string: " << str << " (possible truncation)";
    }
}

babelwires::Fixed::PartialParseResult babelwires::Fixed::partialParse(std::string_view str, Fixed& valueOut,
                                                                      int expectedPrecision) {
    assert(expectedPrecision >= -1);
    assert(expectedPrecision <= s_maxPrecision);
    if (str.empty()) {
        return PartialParseResult::Failure;
    }

    constexpr Fixed::NativeType max = std::numeric_limits<Fixed::NativeType>::max();
    constexpr Fixed::NativeType min = std::numeric_limits<Fixed::NativeType>::min();
    constexpr Fixed::NativeType posGuard = max / 10;
    constexpr Fixed::NativeType negGuard = min / 10;
    Fixed::NativeType value = 0;
    int precision = -1;
    bool isPositive = true;
    bool isOverflow = false;

    if (str[0] == '-') {
        isPositive = false;
        str.remove_prefix(1);
    } else if (str[0] == '+') {
        str.remove_prefix(1);
    }
    if (str.empty()) {
        return PartialParseResult::Truncated;
    }

    std::size_t i = 0;
    for (; i < str.size(); ++i) {
        const char c = str[i];
        if (c == '.') {
            if (precision != -1) {
                return PartialParseResult::Failure;
            }
            if (i == 0) {
                return PartialParseResult::Failure;
            }
            if (i == str.size() - 1) {
                return PartialParseResult::Truncated;
            }
            precision = 0;
            continue;
        } else if (!isDigit(c)) {
            return PartialParseResult::Failure;
        }
        if (isPositive && (value > posGuard)) {
            isOverflow = true;
            break;
        } else if (!isPositive && (value < negGuard)) {
            isOverflow = true;
            break;
        }
        const Fixed::NativeType digit = static_cast<Fixed::NativeType>(c - '0');
        if (precision == Fixed::s_maxPrecision) {
            isOverflow = true;
            break;
        }
        value = value * 10;
        if (precision != -1) {
            ++precision;
        }
        if (isPositive && (value > max - digit)) {
            value = max;
            isOverflow = true;
            break;
        } else if (!isPositive && (value < min + digit)) {
            value = min;
            isOverflow = true;
            break;
        }
        value = value + (isPositive ? digit : -digit);
    }

    if (isOverflow) {
        for (std::size_t j = i; j < str.size(); ++j) {
            const char c = str[j];
            if ((c == '.') && (precision != -1)) {
                return PartialParseResult::Failure;
            } else if (!isDigit(c)) {
                return PartialParseResult::Failure;
            }
        }

        if (precision == -1) {
            if (expectedPrecision == -1) {
                return PartialParseResult::Failure;
            } else if (isPositive) {
                valueOut = Fixed(max, 0);
                return PartialParseResult::Success;
            } else {
                valueOut = Fixed(min, 0);
                return PartialParseResult::Success;
            }
        }

        // We may be able to extract one more digit of precision from the next character.
        if ((i < str.size() - 1) && (str[i + 1] > '5')) {
            if (isPositive && (value < max)) {
                ++value;
            } else if (!isPositive && (value > min)) {
                --value;
            }
        }
        valueOut = Fixed(value, precision);
        if (expectedPrecision >= 0) {
            valueOut = tryFromFixed(valueOut, expectedPrecision);
        }
        return PartialParseResult::Success;
    }

    Fixed valueAsParsed(value, (precision == -1) ? 0 : precision);
    if (expectedPrecision >= 0) {
        valueOut = tryFromFixed(valueAsParsed, expectedPrecision);
    } else {
        valueOut = valueAsParsed;
    }

    return PartialParseResult::Success;
}

std::size_t babelwires::Fixed::getHash() const {
    return hash::mixtureOf(m_numerator, m_precision);
}
