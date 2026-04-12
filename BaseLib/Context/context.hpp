/**
 * A Context is a dynamic, type-indexed service provider.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <cassert>
#include <typeindex>
#include <unordered_map>

namespace babelwires {

    /// A dynamic, type-indexed service provider.
    /// Services are stored in a map keyed by std::type_index, which is guaranteed
    /// to work correctly across shared library boundaries.
    class BASELIB_API Context {
      public:
        /// Register a service of type T. The caller must ensure the service outlives this Context.
        template <typename T>
        void registerService(T& service) {
            auto key = std::type_index(typeid(T));
            assert((m_services.find(key) == m_services.end()) && "Service already registered");
            m_services[key] = &service;
        }

        /// Get a registered service of type T (non-const). Asserts if not registered.
        template <typename T>
        T& getService() {
            T* s = tryGetService<T>();
            assert(s && "Service not registered");
            return *s;
        }

        /// Get a registered service of type T (const). Asserts if not registered.
        template <typename T>
        const T& getService() const {
            const T* s = tryGetService<T>();
            assert(s && "Service not registered");
            return *s;
        }

        /// Try to get a registered service of type T (non-const). Returns nullptr if not registered.
        template <typename T>
        T* tryGetService() {
            auto it = m_services.find(std::type_index(typeid(T)));
            if (it == m_services.end()) {
                return nullptr;
            }
            return static_cast<T*>(it->second);
        }

        /// Try to get a registered service of type T (const). Returns nullptr if not registered.
        template <typename T>
        const T* tryGetService() const {
            auto it = m_services.find(std::type_index(typeid(T)));
            if (it == m_services.end()) {
                return nullptr;
            }
            return static_cast<const T*>(it->second);
        }

        /// Assert that all the specified service types have been registered.
        template <typename... Ts>
        void assertServicesRegistered() const {
            (assertServiceRegistered<Ts>(), ...);
        }

      private:
        template <typename T>
        void assertServiceRegistered() const {
            assert(tryGetService<T>() && "Required service not registered");
        }

        std::unordered_map<std::type_index, void*> m_services;
    };

} // namespace babelwires
