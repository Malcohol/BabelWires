/**
 * A Context is expected to provide some context specific data during high-level serialization
 * and deserialization.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>
#include <BaseLib/Context/context.hpp>

namespace babelwires {
    /// A dynamic, type-indexed service provider which provides context-specific
    /// data during serialization, deserialization, and project operations.
    struct BASELIB_API Context {
        Context m_context;

        template <typename T>
        void registerService(T& service) { m_context.registerService<T>(service); }

        template <typename T>
        T& getService() { return m_context.getService<T>(); }

        template <typename T>
        const T& getService() const { return m_context.getService<T>(); }

        template <typename T>
        T* tryGetService() { return m_context.tryGetService<T>(); }

        template <typename T>
        const T* tryGetService() const { return m_context.tryGetService<T>(); }

        template <typename... Ts>
        void assertServicesRegistered() const { m_context.assertServicesRegistered<Ts...>(); }
    };
} // namespace babelwires
