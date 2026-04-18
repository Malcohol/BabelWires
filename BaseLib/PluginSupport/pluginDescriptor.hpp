/**
 * Plugin descriptor contract for runtime-loadable plugins.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>
#include <BaseLib/Version/version.hpp>

#include <cstddef>

namespace babelwires {
    class Context;
    struct UserLogger;

    /// Descriptor filled in by every plugin's entry point.
    struct PluginDescriptor {
        /// The codebase version of BabelWires that the plugin was built against.
        /// The loader checks Version::getCodebaseVersion().satisfies(m_codebaseVersion).
        Version m_codebaseVersion;

        /// Write the plugin's build fingerprint into the provided buffer.
        /// Returns the number of bytes used (including null terminator).
        /// If the return value equals bufferSize, the output was truncated. The caller can retry with a larger buffer.
        std::size_t (*getBuildFingerprint)(char* buffer, std::size_t bufferSize) = nullptr;

        /// Register the plugin's functionality (e.g. factories) into the Context.
        /// Returns an error if registration fails.
        Result (*registerPlugin)(Context& context, UserLogger& userLogger) = nullptr;
    };

    /// The type of the single plugin entry point, which must be exported by every plugin.
    using GetPluginDescriptorFunction = void (*)(PluginDescriptor* out);

    /// The symbol of the single plugin entry point, which must be exported by every plugin.
    inline constexpr const char* c_pluginDescriptorSymbolName = "babelwires_getPluginDescriptor";

} // namespace babelwires

#if defined(_WIN32) || defined(_WIN64)
    #define BABELWIRES_PLUGIN_EXPORT __declspec(dllexport)
#else
    #define BABELWIRES_PLUGIN_EXPORT
#endif
