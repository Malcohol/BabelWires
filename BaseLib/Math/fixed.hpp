/**
 * Holds a fixed-point value with explicit precision.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Result/result.hpp>

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

namespace babelwires {

    class BASELIB_API Fixed {
      public:
        using NativeType = std::int64_t;

        static constexpr int s_maxPrecision = 12;

        Fixed();
        Fixed(NativeType numerator, int precision = 0);

        /// Convert a Fixed to a different precision while preserving the exact value if possible.
        static ResultT<Fixed> fromFixed(Fixed value, int precision);

        /// Convert a Fixed to a different precision, rounding to the nearest representable value.
        static Fixed tryFromFixed(Fixed value, int precision);

        /// Convert a Fixed to a different precision, asserting that the exact value is representable.
        static Fixed assertFromFixed(Fixed value, int precision);

        /// Convert a floating-point value to Fixed with the given precision.
        static ResultT<Fixed> fromDouble(double value, int precision);

        /// Convert a floating-point value to Fixed, clamping if out of range.
        static Fixed tryFromDouble(double value, int precision);

        /// Convert a floating-point value to Fixed, asserting if conversion fails.
        static Fixed assertFromDouble(double value, int precision);

        NativeType getNumerator() const;
        int getPrecision() const;

        void setNumerator(NativeType numerator);
        void setPrecision(int precision);

        bool operator==(const Fixed& other) const;
        bool operator!=(const Fixed& other) const;

        // Ordering not permitted between Fixed values of different precision.
        bool operator<(const Fixed& other) const;
        bool operator>(const Fixed& other) const;
        bool operator<=(const Fixed& other) const;
        bool operator>=(const Fixed& other) const;

        std::string toString() const;
        double toDouble() const;
        std::string serializeToString() const { return toString(); }

        static ResultT<Fixed> deserializeFromString(std::string_view str);

        enum class PartialParseResult { Success, Failure, Truncated };

        /// If expectedPrecision is specified, then the algorithm tries to return a value with that precision, clamping and truncating if necessary.
        /// Otherwise, it infers the precision from the position of the decimal point. Overflow is a failure but too much precision isn't.
        static PartialParseResult partialParse(std::string_view str, Fixed& valueOut, int expectedPrecision = -1);

        static ResultT<Fixed> parse(std::string_view str) { return deserializeFromString(str); }

        std::size_t getHash() const;

      private:
        NativeType m_numerator = 0;
        int m_precision = 0;
    };

    inline std::ostream& operator<<(std::ostream& os, const Fixed& fixed) {
        return os << fixed.toString();
    }

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::Fixed> {
        inline std::size_t operator()(const babelwires::Fixed& fixed) const { return fixed.getHash(); }
    };
} // namespace std
