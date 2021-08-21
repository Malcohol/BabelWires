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

    template <typename T> struct Range {
        inline Range(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
            : m_min(min)
            , m_max(max) {}
        bool contains(T value) const { return (m_min <= value) && (value <= m_max); }
        T m_min;
        T m_max;
    };

    /// A simple struct representing an iterable range.
    template <typename ITERATOR> struct Span {
        ITERATOR m_begin;
        ITERATOR m_end;

        ITERATOR begin() const { return m_begin; }
        ITERATOR end() const { return m_end; }
    };

    //

    /// Is str usable as an "identifier" ([a..zA..Z][_a..zA..Z0..9]*).
    /// This constraint is just used to keep certain strings sane.
    bool isValidIdentifier(const char* str);

    // Utility templates.

    /// U with the same constness as T.
    template <typename T, typename U> struct CopyConst {
        typedef typename std::conditional<std::is_const<T>::value, typename std::add_const<U>::type, U>::type type;
    };
} // namespace babelwires

/// Adds "asA" methods to a hierarchy, allowing limited downcasting within the hierarchy.
/// This neatens up dynamic_casting in client code, and should make refactors easier in future.
/// foo.asA<BAR>() either returns a BAR* or nullptr.
#define DOWNCASTABLE_TYPE_HIERARCHY(BASE) \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr> \
    T* asA() { return dynamic_cast<T*>(this); } \
    template <typename T, std::enable_if_t<std::is_base_of_v<BASE, T>, std::nullptr_t> = nullptr> \
    const T* asA() const { return dynamic_cast<const T*>(this); }
