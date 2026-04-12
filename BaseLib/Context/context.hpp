/**
 * A Context is a simple type-indexed service provider.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>
#include <BaseLib/Utilities/classUniqueString.hpp>

#include <cassert>
#include <cstdint>
#include <string_view>
#include <vector>

namespace babelwires {

    /// A simple type-indexed service provider.
    /// Does not provide any ownership or lifetime guarantees for the services it holds.
    class BASELIB_API Context {
      public:
        /// Get a registered service of type T (non-const). Asserts if not registered.
        template <typename T> T& get() {
            const std::uint32_t serviceId = getServiceId<T>();
            assert(serviceId < m_services.size() && "Service not registered");
            void* servicePtr = m_services[serviceId];
            assert(servicePtr && "Service not registered");
            return *static_cast<T*>(servicePtr);
        }

        /// Get a registered service of type T (const). Asserts if not registered.
        template <typename T> const T& get() const {
            const std::uint32_t serviceId = getServiceId<T>();
            assert(serviceId < m_services.size() && "Service not registered");
            void* servicePtr = m_services[serviceId];
            assert(servicePtr && "Service not registered");
            return *static_cast<const T*>(servicePtr);
        }
        
        /// Register a service of type T. The caller must ensure the service outlives this Context.
        template <typename T> void registerService(T& service) {
            const std::uint32_t serviceId = getServiceId<T>();
            if (serviceId >= m_services.size()) {
                m_services.resize(serviceId + 1, nullptr);
            }
            assert(!m_services[serviceId] && "Service already registered");
            m_services[serviceId] = &service;
        }

      private:
        /// Get the service ID for type T. This is a static method which returns the same ID for the same type T.
        template <typename T> static std::uint32_t getServiceId() {
            static const std::uint32_t id = getServiceIdForService(getClassUniqueString<T>());
            return id;
        }

        /// Obtain a unique service ID for a given service name. IDs are assigned sequentially starting from 0.
        /// This method is thread-safe, and will return the same ID for the same service name across different
        /// Context instances.
        static std::uint32_t getServiceIdForService(std::string_view serviceName);

      private:
        /// The services stored in this Context, indexed by their service ID. A nullptr value indicates
        /// a service that is known to the system but is not registered in this Context.
        std::vector<void*> m_services;
    };

} // namespace babelwires
