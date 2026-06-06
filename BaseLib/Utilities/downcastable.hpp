/**
 * Downcastable hierarchy macro.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Utilities/classUniqueString.hpp>

#include <cassert>
#include <type_traits>

// Helper macro - CLASS must be the concrete class being defined.
// Implementation note: Since the number of classes in a hierarchy is typically small, we assume that the risk of hash
// collisions is negligible.
#define DOWNCASTABLE_COMMON(CLASS)                                                                                     \
    static constexpr std::uint64_t getHierarchyIdStatic() {                                                            \
        return babelwires::hash::stableStringHash(babelwires::getClassUniqueString<CLASS>());                          \
    }

/// This macro is used in the base class of a hierarchy and provides support for downcasting between classes in the
/// hierarchy. Adds "as" and "tryAs" methods to a hierarchy.
/// "tryAs" returns a pointer, nullptr if the cast is invalid.
/// "as" returns a reference, asserting that the cast is correct.
/// Use of RTTI features in not permitted in the codebase.
#define DOWNCASTABLE_BASE(BASE)                                                                                        \
    DOWNCASTABLE_COMMON(BASE)                                                                                          \
    virtual bool isA(std::uint64_t hierarchyId) const {                                                                \
        return hierarchyId == getHierarchyIdStatic();                                                                  \
    }                                                                                                                  \
    virtual std::uint64_t getHierarchyId() const {                                                                     \
        return getHierarchyIdStatic();                                                                                 \
    }                                                                                                                  \
    template <typename T> T* tryAs() {                                                                                 \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        return isA(T::getHierarchyIdStatic()) ? static_cast<T*>(this) : nullptr;                                       \
    }                                                                                                                  \
    template <typename T> const T* tryAs() const {                                                                     \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        return isA(T::getHierarchyIdStatic()) ? static_cast<const T*>(this) : nullptr;                                 \
    }                                                                                                                  \
    template <typename T> T& as() {                                                                                    \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        assert(isA(T::getHierarchyIdStatic()));                                                                        \
        return static_cast<T&>(*this);                                                                                 \
    }                                                                                                                  \
    template <typename T> const T& as() const {                                                                        \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        assert(isA(T::getHierarchyIdStatic()));                                                                        \
        return static_cast<const T&>(*this);                                                                           \
    }

/// Every class in the hierarchy (except the base) should use this macro, passing the class itself and its parent
/// classes as arguments.
#define DOWNCASTABLE(CLASS, PARENT)                                                                                    \
    DOWNCASTABLE_COMMON(CLASS)                                                                                         \
    bool isA(std::uint64_t hierarchyId) const override {                                                               \
        return (hierarchyId == getHierarchyIdStatic()) || PARENT::isA(hierarchyId);                                    \
    }                                                                                                                  \
    std::uint64_t getHierarchyId() const override {                                                                    \
        return getHierarchyIdStatic();                                                                                 \
    }
