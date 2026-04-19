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
    using WriteBuildFingerprintFunction = std::size_t(BABELWIRES_PLUGIN_CALL*)(char* buffer, std::size_t bufferSize);

    /// Post-validation plugin registration function type.
    using RegisterPluginFunction = Result(BABELWIRES_PLUGIN_CALL*)(Context& context, UserAdvisoryLogger& userLogger);

    /// Simple descriptor intended to support compatibility checking before any rich C++ plugin interaction
    /// occurs.
    ///
    /// Assuming the dynamic linker has managed to load the plugin and find the probe symbol, this provides a way to
    /// check ABI compatibility before any subsequent C++ code is executed. Static initialization in an incompatible
    /// plugin could already have caused problems, but that cannot be avoided entirely.
    struct PluginProbeDescriptor {
        /// This is initialized to the host magic number so it can be checked by the plugin.
        /// Then it is overwritten by the plugin magic number so it can be checked by the host.
        std::uint32_t m_magicNumberInOut = 0;

        /// Future-proofing: This is initialized to the host probe version. The plugin will then set it to the minimum
        /// of its own and host's values (i.e. the largest version they both understand). This allows the plugin to
        /// populate descriptor fields in a way that both-sides understand. Forward and backwards compatibility of
        /// plugins is not expected: The intention is just to obtain a valid descriptor.
        std::uint16_t m_probeVersionInOut = 0;

        /// Future-proofing: This is initialized to the size of the struct as known by the host. The plugin may not set
        /// it to a larger value, but may set it to a smaller value that matches the probe version in the previous
        /// field.
        std::uint32_t m_structSizeInOut = 0;

        std::uint16_t m_codebaseMajor = 0;
        std::uint16_t m_codebaseMinor = 0;
        std::uint16_t m_codebasePatch = 0;

        std::uint8_t m_pluginUuidText[c_pluginUuidTextSize] = {};

        WriteBuildFingerprintFunction writeBuildFingerprint = nullptr;
    };

    /// Probe entry point type exported by every plugin.
    /// Returns 0 on success, non-zero on failure.
    using GetPluginProbeDescriptorFunction = int(BABELWIRES_PLUGIN_CALL*)(PluginProbeDescriptor* out);

    inline constexpr std::uint32_t c_pluginProbeHostMagicNumber = 0x126ec476u;
    inline constexpr std::uint32_t c_pluginProbePluginMagicNumber = 0x2eff60f9u;
    inline constexpr std::uint16_t c_pluginProbeAbiVersion = 1;

    /// Symbol names exported by every plugin.
    inline constexpr const char* c_pluginProbeDescriptorSymbolName = "babelwires_getPluginProbeDescriptor";
    inline constexpr const char* c_pluginRegistrationSymbolName = "babelwires_registerPlugin";

    static_assert(std::is_standard_layout_v<PluginProbeDescriptor>);
    static_assert(std::is_trivially_copyable_v<PluginProbeDescriptor>);

} // namespace babelwires
