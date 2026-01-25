/**
 * Rational is a very rudimentary implementation of rational numbers.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Math/math.hpp>
#include <BaseLib/Utilities/result.hpp>

#include <cctype>
#include <cstdint>
#include <limits>
#include <ostream>

namespace babelwires {

    /// Very rudimentary implementation of rational numbers.
    class Rational {
      public:
        using ComponentType = std::int32_t;

        constexpr Rational(ComponentType numerator = 0)
            : m_numerator(numerator)
            , m_denominator(1) {}
        Rational(ComponentType numerator, ComponentType denominator);

        ComponentType getNumerator() const { return m_numerator; }
        ComponentType getDenominator() const { return m_denominator; }

        /// These can throw a MathException if the result is not representable.
        Rational operator*(const Rational& other) const;
        Rational operator/(const Rational& other) const;
        Rational operator+(const Rational& other) const;
        Rational operator-(const Rational& other) const;
        Rational& operator+=(const Rational& other);
        Rational& operator-=(const Rational& other);

        Rational operator-() const;

        bool operator<=(const Rational& other) const;
        bool operator<(const Rational& other) const;
        bool operator>=(const Rational& other) const;
        bool operator>(const Rational& other) const;
        bool operator==(const Rational& other) const;
        bool operator!=(const Rational& other) const;

        /// A printable string. If necessary, this will be a compound fraction (i.e. 1 1/2).
        std::string toString() const;

        /// A string that exploits HTML sup and sub to present fractions nicely.
        /// If necessary, this will be a compound fraction (i.e. 1 1/2).
        std::string toHtmlString() const;

        enum class PartialParseResult { Success, Failure, Truncated };

        static PartialParseResult partialParse(std::string_view str, Rational& valueOut);

        /// Serialization
        std::string serializeToString() const { return toString(); }
        static ResultT<Rational> deserializeFromString(std::string_view str);

        /// Get a hash value for the rational.
        std::size_t getHash() const;

        /// Return (d, m) where d*x + m = this and m is smallest.
        /// Note: The current implementation can return negative remainders for negative numbers.
        std::tuple<int, Rational> divmod(Rational x) const;

      private:
        using BigType = std::int64_t;
        void setComponents(BigType numerator, BigType denominator);

      private:
        ComponentType m_numerator;
        ComponentType m_denominator;
    };

    /// n/d
    std::ostream& operator<<(std::ostream& os, const babelwires::Rational& r);

} // namespace babelwires

namespace std {
    /// Specialize numeric_limits for Rational.
    template <> class numeric_limits<babelwires::Rational> {
      public:
        static constexpr bool isSpecialized = true;
        static constexpr babelwires::Rational min() {
            return babelwires::Rational(numeric_limits<babelwires::Rational::ComponentType>::min());
        }
        static constexpr babelwires::Rational max() {
            return babelwires::Rational(numeric_limits<babelwires::Rational::ComponentType>::max());
        }
        // TODO All the other stuff.
    };

    template <> struct hash<babelwires::Rational> {
        inline std::size_t operator()(const babelwires::Rational& rat) const { return rat.getHash(); }
    };
} // namespace std
