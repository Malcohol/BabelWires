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
        std::string serializeToString() const { return toString(); }

        static ResultT<Fixed> deserializeFromString(std::string_view str);
        static ResultT<Fixed> parse(std::string_view str) { return deserializeFromString(str); }

        std::size_t getHash() const;

      private:
        static int normalizePrecision(int precision);
        static std::uint64_t pow10(int precision);
        static std::uint64_t abs64(NativeType value);

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
