/**
 * Rational is a very rudimentary implementation of rational numbers.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/Math/rational.hpp>

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/exceptions.hpp>

#include <cassert>
#include <functional>
#include <sstream>

void babelwires::Rational::setComponents(BigType numerator, BigType denominator) {
    assert(denominator != 0);
    BigType sign = 1;
    if ((numerator * denominator) < 0) {
        sign = -1;
        numerator = -numerator;
    }
    if (numerator == 0) {
        numerator = 0;
        denominator = 1;
    } else {
        BigType g = gcd(numerator, denominator);
        numerator = numerator / g;
        denominator = denominator / g;
    }
    numerator *= sign;
    assert(denominator > 0);
    if ((numerator > std::numeric_limits<ComponentType>::max()) ||
        (denominator > std::numeric_limits<ComponentType>::max()) ||
        (numerator < std::numeric_limits<ComponentType>::min())) {
        // TODO: Consider approximating the result, when possible.
        throw MathException() << "Unrepresentable rational";
    }
    m_numerator = static_cast<ComponentType>(numerator);
    m_denominator = static_cast<ComponentType>(denominator);
}

babelwires::Rational::Rational(ComponentType numerator, ComponentType denominator) {
    assert(denominator != 0);
    setComponents(numerator, denominator);
}

babelwires::Rational babelwires::Rational::operator*(const Rational& other) const {
    Rational result;
    result.setComponents(static_cast<BigType>(m_numerator) * other.m_numerator,
                         static_cast<BigType>(m_denominator) * other.m_denominator);
    return result;
}

babelwires::Rational babelwires::Rational::operator/(const Rational& other) const {
    Rational result;
    result.setComponents(static_cast<BigType>(m_numerator) * other.m_denominator,
                         static_cast<BigType>(m_denominator) * other.m_numerator);
    return result;
}

babelwires::Rational& babelwires::Rational::operator+=(const Rational& other) {
    BigType numerator = (m_numerator * other.m_denominator) + (m_denominator * other.m_numerator);
    BigType denominator = m_denominator * other.m_denominator;
    setComponents(numerator, denominator);
    return *this;
}

babelwires::Rational babelwires::Rational::operator+(const Rational& other) const {
    Rational tmp = *this;
    tmp += other;
    return tmp;
}

babelwires::Rational babelwires::Rational::operator-() const {
    Rational result;
    result.setComponents(-static_cast<BigType>(m_numerator), m_denominator);
    return result;
}

babelwires::Rational& babelwires::Rational::operator-=(const Rational& other) {
    *this += -other;
    return *this;
}

babelwires::Rational babelwires::Rational::operator-(const Rational& other) const {
    Rational tmp = *this;
    tmp -= other;
    return tmp;
}

bool babelwires::Rational::operator==(const Rational& other) const {
    return (other.m_numerator == m_numerator) && (other.m_denominator == m_denominator);
}

bool babelwires::Rational::operator!=(const Rational& other) const {
    return !(*this == other);
}

bool babelwires::Rational::operator<(const Rational& other) const {
    return static_cast<BigType>(m_numerator) * static_cast<BigType>(other.m_denominator) <
           static_cast<BigType>(other.m_numerator) * static_cast<BigType>(m_denominator);
}

bool babelwires::Rational::operator>(const Rational& other) const {
    return other < *this;
}

bool babelwires::Rational::operator<=(const Rational& other) const {
    return (*this == other) || (*this < other);
}

bool babelwires::Rational::operator>=(const Rational& other) const {
    return (*this == other) || (other < *this);
}

std::string babelwires::Rational::toString() const {
    std::ostringstream result;
    if (m_denominator == 1) {
        result << m_numerator;
    } else if (abs(m_numerator) < m_denominator) {
        result << m_numerator << "/" << m_denominator;
    } else {
        result << (m_numerator / m_denominator) << " " << (abs(m_numerator) % m_denominator) << "/" << m_denominator;
    }
    return result.str();
}

std::string babelwires::Rational::toHtmlString() const {
    std::ostringstream result;
    if (m_denominator == 1) {
        result << m_numerator;
    } else if (abs(m_numerator) < m_denominator) {
        result << "<sup>" << m_numerator << "</sup>&frasl;<sub>" << m_denominator << "</sub>";
    } else {
        result << (m_numerator / m_denominator) << "<sup>" << (abs(m_numerator) % m_denominator) << "</sup>&frasl;<sub>"
               << m_denominator << "</sub>";
    }
    return result.str();
}

std::ostream& babelwires::operator<<(std::ostream& os, const babelwires::Rational& r) {
    return os << r.toString();
}

babelwires::Rational::PartialParseResult babelwires::Rational::partialParse(std::string_view str, Rational& valueOut) {
    auto it = str.begin();
    if (it == str.end()) {
        return PartialParseResult::Truncated;
    }

    auto skipWhitespace = [&it, str]() {
        while ((it != str.end()) && (*it == ' ')) {
            ++it;
        }
    };

    skipWhitespace();

    if (it == str.end()) {
        return PartialParseResult::Truncated;
    }

    bool isNegative = false;
    if (*it == '-') {
        isNegative = true;
        ++it;
        skipWhitespace();
        if (it == str.end()) {
            return PartialParseResult::Truncated;
        }
    }

    enum { NO_DIGITS = -1, DIGITS_BUT_ERROR = -2 };

    // -1 No number
    // -2 Error
    auto parseNumber = [&it, str]() -> BigType {
        BigType number = NO_DIGITS;
        bool isZero = false;
        // Parse first digit specially.
        while (it != str.end()) {
            const char c = *it;
            if ((c >= '0') && (c <= '9')) {
                if (isZero) {
                    return DIGITS_BUT_ERROR;
                } else if (c == '0') {
                    number = 0;
                    isZero = true;
                    ++it;
                } else {
                    number = c - '0';
                    ++it;
                    break;
                }
            } else {
                return number;
            }
        }
        // Parse subsequent digits.
        while (it != str.end()) {
            const char c = *it;
            if (number > std::numeric_limits<ComponentType>::max()) {
                return DIGITS_BUT_ERROR;
            } else if ((c >= '0') && (c <= '9')) {
                number = (number * 10) + (c - '0');
                ++it;
            } else {
                break;
            }
        }
        return number;
    };

    const BigType firstNumber = parseNumber();

    if (firstNumber < 0) {
        return PartialParseResult::Failure;
    }

    skipWhitespace();

    if (it == str.end()) {
        valueOut =
            Rational(isNegative ? -static_cast<ComponentType>(firstNumber) : static_cast<ComponentType>(firstNumber));
        return PartialParseResult::Success;
    }

    bool mixedNumber = true;

    if (*it == '/') {
        ++it;
        mixedNumber = false;
        skipWhitespace();
    }

    const BigType secondNumber = parseNumber();

    if ((secondNumber == NO_DIGITS) && (it == str.end())) {
        return PartialParseResult::Truncated;
    } else if (secondNumber <= 0) {
        return PartialParseResult::Failure;
    }

    skipWhitespace();

    if (!mixedNumber) {
        if (it == str.end()) {
            valueOut = Rational(
                (isNegative ? -static_cast<ComponentType>(firstNumber) : static_cast<ComponentType>(firstNumber)),
                static_cast<ComponentType>(secondNumber));
            return PartialParseResult::Success;
        } else {
            // Trailing characters.
            return PartialParseResult::Failure;
        }
    }

    if (it == str.end()) {
        return PartialParseResult::Truncated;
    } else if (*it != '/') {
        return PartialParseResult::Failure;
    }
    ++it;
    skipWhitespace();

    const BigType thirdNumber = parseNumber();
    if ((thirdNumber == NO_DIGITS) && (it == str.end())) {
        return PartialParseResult::Truncated;
    } else if (thirdNumber <= 0) {
        return PartialParseResult::Failure;
    }

    skipWhitespace();

    if (it != str.end()) {
        // Trailing characters.
        return PartialParseResult::Failure;
    }

    const BigType numerator = (isNegative ? -1 : 1) * ((firstNumber * thirdNumber) + secondNumber);

    if ((numerator < std::numeric_limits<ComponentType>::lowest()) ||
        (numerator < std::numeric_limits<ComponentType>::min())) {
        return PartialParseResult::Failure;
    }

    valueOut = Rational(static_cast<ComponentType>(numerator), static_cast<ComponentType>(thirdNumber));

    return PartialParseResult::Success;
}

babelwires::ResultT<babelwires::Rational> babelwires::Rational::deserializeFromString(std::string_view str) {
    Rational value;
    switch (partialParse(str, value)) {
        case PartialParseResult::Success:
            return value;
        case PartialParseResult::Failure:
            return Error() << "Could not parse \"" << str << "\" as a rational number";
        case PartialParseResult::Truncated:
            return Error() << "Could not parse \"" << str << "\" as a rational number. Possible truncation";
    }
    return value;
}

std::size_t babelwires::Rational::getHash() const {
    return hash::mixtureOf(m_numerator, m_denominator);
}

std::tuple<int, babelwires::Rational> babelwires::Rational::divmod(Rational x) const {
    const ComponentType e = x.m_denominator * m_numerator;
    const ComponentType a = e / m_denominator;
    const ComponentType b = e % m_denominator;
    const ComponentType u = a / x.m_numerator;
    const ComponentType v = a % x.m_numerator;
    const Rational mod = (Rational(v) + Rational(b, m_denominator)) / x.m_denominator; 
    return { u, mod };
}
