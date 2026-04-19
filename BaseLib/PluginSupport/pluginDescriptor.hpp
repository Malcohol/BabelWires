/**
 * Plugin descriptor contract for runtime-loadable plugins.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>

#include <cstddef>
#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
    #define BABELWIRES_PLUGIN_EXPORT __declspec(dllexport)
    #define BABELWIRES_PLUGIN_CALL __cdecl
#else
    #define BABELWIRES_PLUGIN_EXPORT __attribute__((visibility("default")))
    #define BABELWIRES_PLUGIN_CALL
#endif

namespace babelwires {
    class Context;
    struct UserAdvisoryLogger;

    inline constexpr std::size_t c_pluginUuidTextSize = 36;

    /// Function type used during the probe stage to fetch the plugin build fingerprint.
    using WriteBuildFingerprintFunction = std::size_t (BABELWIRES_PLUGIN_CALL *)(char* buffer, std::size_t bufferSize);

    /// Post-validation plugin registration function type.
    using RegisterPluginFunction = Result (BABELWIRES_PLUGIN_CALL *)(Context& context, UserAdvisoryLogger& userLogger);

    /// ABI-simple descriptor returned before any rich C++ plugin interaction occurs.
    struct PluginProbeDescriptor {
        std::uint32_t m_magic = 0;
        std::uint16_t m_abiVersion = 0;
        std::uint16_t m_reserved = 0;
        std::uint32_t m_structSize = 0;

        std::uint16_t m_codebaseMajor = 0;
        std::uint16_t m_codebaseMinor = 0;
        std::uint16_t m_codebasePatch = 0;
        std::uint16_t m_flags = 0;

        std::uint8_t m_pluginUuidText[c_pluginUuidTextSize] = {};

        WriteBuildFingerprintFunction writeBuildFingerprint = nullptr;
    };

    /// Probe entry point type exported by every plugin.
    using GetPluginProbeDescriptorFunction = void (BABELWIRES_PLUGIN_CALL *)(PluginProbeDescriptor* out);

    inline constexpr std::uint32_t c_pluginProbeMagic = 0x42575044u;
    inline constexpr std::uint16_t c_pluginProbeAbiVersion = 1;

    /// Symbol names exported by every plugin.
    inline constexpr const char* c_pluginProbeDescriptorSymbolName = "babelwires_getPluginProbeDescriptor";
    inline constexpr const char* c_pluginRegistrationSymbolName = "babelwires_registerPlugin";

} // namespace babelwires
