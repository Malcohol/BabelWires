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

#define DOWNCASTABLE_COMMON                                                                                            \
    static const void* getHierarchyId() {                                                                              \
        static const char id = 0;                                                                                      \
        return &id;                                                                                                    \
    }

/// This macro is used in the base class of a hierarchy and provides support
/// for downcasting between classes in the hierarchy.
/// Adds "as" and "tryAs" methods to a hierarchy.
/// "tryAs" provides a checked downcast within the hierarchy.
/// foo.tryAs<BAR>() either returns a BAR* or nullptr.
/// "as" asserts that the downcast is correct.
/// Use of dynamic_cast (and other RTTI features) is not permitted in the codebase.
#define DOWNCASTABLE_BASE(BASE)                                                                                        \
    DOWNCASTABLE_COMMON                                                                                                \
    virtual bool isA(const void* hierarchyId) const {                                                                  \
        return hierarchyId == getHierarchyId();                                                                        \
    }                                                                                                                  \
    template <typename T> T* tryAs() {                                                                                 \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        return isA(T::getHierarchyId()) ? static_cast<T*>(this) : nullptr;                                             \
    }                                                                                                                  \
    template <typename T> const T* tryAs() const {                                                                     \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        return isA(T::getHierarchyId()) ? static_cast<const T*>(this) : nullptr;                                       \
    }                                                                                                                  \
    template <typename T> T& as() {                                                                                    \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        assert(isA(T::getHierarchyId()));                                                                              \
        return static_cast<T&>(*this);                                                                                 \
    }                                                                                                                  \
    template <typename T> const T& as() const {                                                                        \
        static_assert(std::is_base_of_v<BASE, T>, "T must belong to this hierarchy");                                  \
        assert(isA(T::getHierarchyId()));                                                                              \
        return static_cast<const T&>(*this);                                                                           \
    }

/// Every class in the hierarchy (except the base) should use this macro,
/// passing the class itself and its parent classes as arguments.
#define DOWNCASTABLE(CLASS, PARENT)                                                                                    \
    DOWNCASTABLE_COMMON                                                                                                \
    bool isA(const void* hierarchyId) const override {                                                                 \
        return (hierarchyId == getHierarchyId()) || PARENT::isA(hierarchyId);                                          \
    }
