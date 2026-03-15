/**
 * Downcastable hierarchy macro.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <cassert>
#include <type_traits>

// Helper macro
#define DOWNCASTABLE_COMMON                                                                                            \
    static const void* getHierarchyIdStatic() {                                                                        \
        static const char id = 0;                                                                                      \
        return &id;                                                                                                    \
    }

/// This macro is used in the base class of a hierarchy and provides support for downcasting between classes in the
/// hierarchy. Adds "as" and "tryAs" methods to a hierarchy.
/// "tryAs" returns a pointer, nullptr if the cast is invalid.
/// "as" returns a reference, asserting that the cast is correct.
/// Use of RTTI features in not permitted in the codebase.
#define DOWNCASTABLE_BASE(BASE)                                                                                        \
    DOWNCASTABLE_COMMON                                                                                                \
    virtual bool isA(const void* hierarchyId) const {                                                                  \
        return hierarchyId == getHierarchyIdStatic();                                                                  \
    }                                                                                                                  \
    virtual const void* getHierarchyId() const {                                                                       \
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

/// Every class in the hierarchy (except the base) should use this macro, passing the class itself and its parent classes as arguments.
#define DOWNCASTABLE(CLASS, PARENT)                                                                                    \
    DOWNCASTABLE_COMMON                                                                                                \
    bool isA(const void* hierarchyId) const override {                                                                 \
        return (hierarchyId == getHierarchyIdStatic()) || PARENT::isA(hierarchyId);                                    \
    }                                                                                                                  \
    const void* getHierarchyId() const override {                                                                      \
        return getHierarchyIdStatic();                                                                                 \
    }
