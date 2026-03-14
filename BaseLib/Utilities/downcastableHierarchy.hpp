/**
 * Downcastable hierarchy macro.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <cassert>
#include <type_traits>

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
