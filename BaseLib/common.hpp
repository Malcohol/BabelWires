/**
 * Some general type definitions, functions and templates.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace babelwires {

    using AudioSample = float;
    using Duration = double;
    using Rate = Duration;
    using Byte = std::uint8_t;
    using VersionNumber = std::uint16_t;

    enum Endianness { IS_BIG_ENDIAN, IS_LITTLE_ENDIAN };

    inline const Endianness getPlatformEndianness() {
        constexpr union {
            char m_bytes[4];
            uint32_t m_int;
        } data = {'\xAA', '\xBB', '\xCC', '\xDD'};

        return (data.m_int == 0xAABBCCDDu) ? IS_BIG_ENDIAN : IS_LITTLE_ENDIAN;
    }

    // TODO: Consider renaming Interval.
    // TODO: Allow ranges to describe open and closed intervals.
    template <typename T> struct Range {
        inline Range(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
            : m_min(min)
            , m_max(max) {}
        bool contains(T value) const { return (m_min <= value) && (value <= m_max); }
        bool contains(const Range& other) const { return contains(other.m_min) && contains(other.m_max); }
        bool operator==(const Range& other) const { return (m_min == other.m_min) && (m_max == other.m_max); }
        bool operator!=(const Range& other) const { return (m_min != other.m_min) || (m_max != other.m_max); }
        T clamp(T value) const { return std::clamp(value, m_min, m_max); }
        T m_min;
        T m_max;
    };

    /// A simple struct representing an iterable range.
    template <typename ITERATOR> struct Span {
        Span(ITERATOR begin, ITERATOR end)
            : m_begin(std::move(begin))
            , m_end(std::move(end)) {}
        ITERATOR m_begin;
        ITERATOR m_end;

        ITERATOR begin() const { return m_begin; }
        ITERATOR end() const { return m_end; }
    };

    template <typename T> auto reverseIterate(T&& iterable) {
        return Span{std::rbegin(iterable), std::rend(iterable)};
    }

    /// Is str usable as an "identifier" ([a..zA..Z][_a..zA..Z0..9]*).
    /// This constraint is just used to keep certain strings sane.
    bool isValidIdentifier(const char* str);

    // Utility templates.

    /// U with the same constness as T.
    template <typename T, typename U> struct CopyConst {
        typedef typename std::conditional<std::is_const<T>::value, typename std::add_const<U>::type, U>::type type;
    };

    template <typename TO, typename FROM> std::unique_ptr<TO> uniquePtrCast(std::unique_ptr<FROM> ptr) {
        return std::unique_ptr<TO>(static_cast<TO*>(ptr.release()));
    }

    /// Standard utilities to allow std::visit to use lambdas.
    template <class... Ts> struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    /// Unified handling of path to string conversion. (I'm deferring Unicode issues to the future.)
    std::string pathToString(const std::filesystem::path& path);
} // namespace babelwires

/// Adds "as" and "tryAs" methods to a hierarchy.
/// "tryAs" provides limited downcasting within the hierarchy.
/// foo.tryAs<BAR>() either returns a BAR* or nullptr.
/// "is" is not a dynamic_cast: it is a shorthand for a static_cast to a T&, although it does assert using "tryAs".
/// Direct use of dynamic_cast (and other RTTI features) is not permitted in the codebase.
#define DOWNCASTABLE_TYPE_HIERARCHY(BASE)                                                                              \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr> T* tryAs() {         \
        return dynamic_cast<T*>(this);                                                                                 \
    }                                                                                                                  \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr>                      \
    const T* tryAs() const {                                                                                           \
        return dynamic_cast<const T*>(this);                                                                           \
    }                                                                                                                  \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr> T& as() {            \
        assert(tryAs<T>());                                                                                            \
        return static_cast<T&>(*this);                                                                                 \
    }                                                                                                                  \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr>                      \
    const T& as() const {                                                                                              \
        assert(tryAs<T>());                                                                                            \
        return static_cast<const T&>(*this);                                                                           \
    }
