/**
 * A Context is a simple type-indexed service provider.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BaseLib/Context/context.hpp>

#include <mutex>
#include <string>
#include <unordered_map>

std::uint32_t babelwires::Context::getServiceIdForService(std::string_view serviceName) {
    static std::mutex s_serviceIdsMutex;
    static std::unordered_map<std::string, std::uint32_t> s_serviceIds;
    static std::uint32_t s_nextServiceId = 0;

    std::lock_guard<std::mutex> lock(s_serviceIdsMutex);
    const auto [it, inserted] = s_serviceIds.emplace(serviceName, s_nextServiceId);
    if (inserted) {
        ++s_nextServiceId;
    }
    return it->second;
}
