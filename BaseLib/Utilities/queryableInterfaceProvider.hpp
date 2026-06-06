/**
 * Queryable interface macros.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Utilities/classUniqueString.hpp>
#include <BaseLib/Utilities/utilityMacros.hpp>

#include <concepts>
#include <cstdint>

namespace babelwires {
    namespace detail {
        template <typename T>
        concept QueryableInterface = requires {
            { T::getInterfaceIdStatic() } -> std::convertible_to<std::uint64_t>;
        };
    } // namespace detail
} // namespace babelwires

#define BW_QUERY_INTERFACE_CASE(INTERFACE)                                                                             \
    if (interfaceId == INTERFACE::getInterfaceIdStatic()) {                                                            \
        return static_cast<INTERFACE*>(this);                                                                          \
    }

#define BW_QUERY_INTERFACE_CASE_CONST(INTERFACE)                                                                       \
    if (interfaceId == INTERFACE::getInterfaceIdStatic()) {                                                            \
        return static_cast<const INTERFACE*>(this);                                                                    \
    }

/// Declare a pure capability interface that can be queried from an owning hierarchy.
// Implementation note: Since the number of interfaces that would ever be queried in one hierarchy is likely very small,
// we assume that the risk of hash collisions is negligible.
#define QUERYABLE_INTERFACE(INTERFACE)                                                                                 \
    static constexpr std::uint64_t getInterfaceIdStatic() {                                                            \
        return babelwires::hash::stableStringHash(babelwires::getClassUniqueString<INTERFACE>());                      \
    }

/// Add support for making capability interfaces available within the hierarchy below a base class.
///
/// Classes using this macro can expose capabilities with tryInterface<T>().
/// Derived classes should use QUERYABLE_INTERFACE_PROVIDER(PARENT, ...) to make the
/// interfaces they implement available.
///
/// Note: The base class cannot itself make an interface available this way, but
/// that's OK because such an interface does not need to be queried dynamically.
#define QUERYABLE_INTERFACE_PROVIDER_BASE()                                                                            \
    template <typename T> T* tryInterface() {                                                                          \
        static_assert(babelwires::detail::QueryableInterface<T>, "T must declare QUERYABLE_INTERFACE");                \
        return static_cast<T*>(queryInterface(T::getInterfaceIdStatic()));                                             \
    }                                                                                                                  \
    template <typename T> const T* tryInterface() const {                                                              \
        static_assert(babelwires::detail::QueryableInterface<T>, "T must declare QUERYABLE_INTERFACE");                \
        return static_cast<const T*>(queryInterface(T::getInterfaceIdStatic()));                                       \
    }                                                                                                                  \
    virtual void* queryInterface(std::uint64_t interfaceId) {                                                          \
        (void)interfaceId;                                                                                             \
        return nullptr;                                                                                                \
    }                                                                                                                  \
    virtual const void* queryInterface(std::uint64_t interfaceId) const {                                              \
        (void)interfaceId;                                                                                             \
        return nullptr;                                                                                                \
    }

/// Make one or more capability interfaces available from a derived class.
///
/// PARENT must already provide QUERYABLE_INTERFACE_PROVIDER_BASE() or QUERYABLE_INTERFACE_PROVIDER(...).
/// Each listed interface must use QUERYABLE_INTERFACE.
/// Supports up to 8 interfaces per class.
#define QUERYABLE_INTERFACE_PROVIDER(PARENT, ...)                                                                      \
    void* queryInterface(std::uint64_t interfaceId) override {                                                         \
        BW_FOR_EACH(BW_QUERY_INTERFACE_CASE, __VA_ARGS__)                                                              \
        return PARENT::queryInterface(interfaceId);                                                                    \
    }                                                                                                                  \
    const void* queryInterface(std::uint64_t interfaceId) const override {                                             \
        BW_FOR_EACH(BW_QUERY_INTERFACE_CASE_CONST, __VA_ARGS__)                                                        \
        return PARENT::queryInterface(interfaceId);                                                                    \
    }
