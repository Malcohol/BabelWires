/**
 * Defines standard operators for a class enum.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <type_traits>

/// Defines the operators |, & and ~ for a class enum.
/// For sanity, it is asserted that the enum has an unsigned underlying type.
#define DEFINE_ENUM_FLAG_OPERATORS(ENUM_FLAGS)                                                                         \
    inline ENUM_FLAGS operator|(ENUM_FLAGS a, ENUM_FLAGS b) {                                                          \
        static_assert(std::is_unsigned_v<std::underlying_type_t<ENUM_FLAGS>>);                                         \
        return static_cast<ENUM_FLAGS>(static_cast<std::underlying_type_t<ENUM_FLAGS>>(a) |                            \
                                       static_cast<std::underlying_type_t<ENUM_FLAGS>>(b));                            \
    }                                                                                                                  \
                                                                                                                       \
    inline ENUM_FLAGS operator&(ENUM_FLAGS a, ENUM_FLAGS b) {                                                          \
        return static_cast<ENUM_FLAGS>(static_cast<std::underlying_type_t<ENUM_FLAGS>>(a) &                            \
                                       static_cast<std::underlying_type_t<ENUM_FLAGS>>(b));                            \
    }                                                                                                                  \
                                                                                                                       \
    inline ENUM_FLAGS operator~(ENUM_FLAGS a) {                                                                        \
        return static_cast<ENUM_FLAGS>(~static_cast<std::underlying_type_t<ENUM_FLAGS>>(a));                           \
    }                                                                                                                  \
                                                                                                                       \
    inline bool isNonzero(ENUM_FLAGS a) { return static_cast<std::underlying_type_t<ENUM_FLAGS>>(a) != 0; }            \
                                                                                                                       \
    inline bool isZero(ENUM_FLAGS a) { return static_cast<std::underlying_type_t<ENUM_FLAGS>>(a) == 0; }
