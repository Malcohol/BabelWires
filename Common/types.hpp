/**
 * Some general type definitions, functions and templates.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>
#include <algorithm>

namespace babelwires {

    using AudioSample = float;
    using Duration = double;
    using Rate = Duration;
    using Byte = std::uint8_t;
    using VersionNumber = std::uint16_t;

    enum Endianness { IS_BIG_ENDIAN, IS_LITTLE_ENDIAN };

    constexpr babelwires::Endianness getPlatformEndianness() {
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
        bool contains(const Range& other) const { 
            return contains(other.m_min) && contains(other.m_max);
        }
        bool operator==(const Range& other) const {
            return (m_min == other.m_min) && (m_max == other.m_max);
        }
        bool operator!=(const Range& other) const {
            return (m_min != other.m_min) || (m_max != other.m_max);
        }
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

    template <typename T> struct ReverseIterateWrapper {
        T& iterable;
    };

    template <typename T> auto begin(ReverseIterateWrapper<T> w) {
        return std::rbegin(w.iterable);
    }

    template <typename T> auto end(ReverseIterateWrapper<T> w) {
        return std::rend(w.iterable);
    }

    template <typename T> ReverseIterateWrapper<T> reverseIterate(T&& iterable) {
        return {iterable};
    }
    //

    /// Is str usable as an "identifier" ([a..zA..Z][_a..zA..Z0..9]*).
    /// This constraint is just used to keep certain strings sane.
    bool isValidIdentifier(const char* str);

    // Utility templates.

    /// U with the same constness as T.
    template <typename T, typename U> struct CopyConst {
        typedef typename std::conditional<std::is_const<T>::value, typename std::add_const<U>::type, U>::type type;
    };

    template<typename BASE, typename DERIVED> 
    std::unique_ptr<BASE> uniquePtrCast(std::unique_ptr<DERIVED> ptr) {
        return std::unique_ptr<BASE>(ptr.release());
    }

    /// Standard utilities to allow std::visit to use lambdas.
    template <class... Ts> struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
} // namespace babelwires

/// Adds "as" and "is" methods to a hierarchy.
/// "as" provides limited downcasting within the hierarchy.
/// foo.as<BAR>() either returns a BAR* or nullptr.
/// "is" is not a dynamic_cast: it is a shorthand for a static_cast to a T&, although it does assert using "as".
/// These should neaten up dynamic_casting in client code, and should make refactors easier in future.
#define DOWNCASTABLE_TYPE_HIERARCHY(BASE)                                                                              \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr> T* as() {            \
        return dynamic_cast<T*>(this);                                                                                 \
    }                                                                                                                  \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr>                      \
    const T* as() const {                                                                                              \
        return dynamic_cast<const T*>(this);                                                                           \
    }                                                                                                                  \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr> T& is() {            \
        assert(as<T>());                                                                                               \
        return static_cast<T&>(*this);                                                                                 \
    }                                                                                                                  \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr>                      \
    const T& is() const {                                                                                              \
        assert(as<T>());                                                                                               \
        return static_cast<const T&>(*this);                                                                           \
    }
