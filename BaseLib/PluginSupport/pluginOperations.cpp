/**
 * Plugin discovery, validation and loading operations.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/PluginSupport/pluginOperations.hpp>

#include <BaseLib/BuildCompatibility/buildCompatibility.hpp>
#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>
#include <BaseLib/Log/userLogger.hpp>
#include <BaseLib/PluginSupport/pluginDescriptor.hpp>
#include <BaseLib/PluginSupport/Detail/pluginModuleOperations.hpp>
#include <BaseLib/Result/resultDSL.hpp>
#include <BaseLib/Version/version.hpp>
#include <BaseLib/uuid.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <string>

namespace {
    babelwires::ResultT<babelwires::Uuid> uuidFromProbeText(const std::uint8_t* uuidTextBytes) {
        std::array<char, babelwires::c_pluginUuidTextSize> uuidText{};
        for (std::size_t index = 0; index < uuidText.size(); ++index) {
            uuidText[index] = static_cast<char>(uuidTextBytes[index]);
        }

        return babelwires::Uuid::deserializeFromString(std::string_view(uuidText.data(), uuidText.size()));
    }
} // namespace

babelwires::ResultT<std::vector<std::filesystem::path>> babelwires::discoverPlugins(const std::filesystem::path& pluginDir, UserLogger& userLogger) {
    constexpr std::size_t c_maxPluginSearchDepth = 8;

    std::error_code ec;
    if (!std::filesystem::exists(pluginDir, ec)) {
        return Error() << "Plugin directory does not exist: " << pluginDir;
    }
    if (ec) {
        return Error() << "Failed to query plugin directory " << pluginDir << ": " << ec.message();
    }

    if (!std::filesystem::is_directory(pluginDir, ec)) {
        return Error() << "Plugin path is not a directory: " << pluginDir;
    }
    if (ec) {
        return Error() << "Failed to inspect plugin directory " << pluginDir << ": " << ec.message();
    }

    std::vector<std::filesystem::path> plugins;
    const auto optionsFind = std::filesystem::directory_options::skip_permission_denied | std::filesystem::directory_options::follow_directory_symlink;
    const auto optionsCheckPermissions = std::filesystem::directory_options::none;
    for (std::filesystem::recursive_directory_iterator it(pluginDir, optionsFind, ec), end; it != end; it.increment(ec)) {
        if (ec) {
            return Error() << "Failed to scan plugin directory " << pluginDir << ": " << ec.message();
        }

        const auto& entry = *it;
        if (entry.is_directory(ec) && !ec && it.depth() >= c_maxPluginSearchDepth) {
            it.disable_recursion_pending();
            userLogger.logWarning() << "Not descending into excessively deep directory " << entry.path() << " when scanning for plugins";
        }
        if (ec) {
            return Error() << "Failed to inspect plugin entry " << entry.path() << ": " << ec.message();
        }

        std::error_code typeError;
        if (entry.is_regular_file(typeError) && (entry.path().extension() == c_pluginFileExtension)) {
            plugins.emplace_back(entry.path());
        }
    }
    // Report permission denied errors 
    for (std::filesystem::recursive_directory_iterator it(pluginDir, optionsCheckPermissions, ec), end; it != end; it.increment(ec)) {
        const auto& entry = *it;
        if (entry.is_directory(ec) && !ec && it.depth() >= c_maxPluginSearchDepth) {
            it.disable_recursion_pending();
        }
        if (ec) {
            userLogger.logWarning() << "Permission denied accessing " << entry.path() << " when scanning for plugins: " << ec.message();
            ec.clear();
        }
    }
    std::sort(plugins.begin(), plugins.end());
    return plugins;
}

babelwires::ResultT<babelwires::PluginHandle> babelwires::openPlugin(const std::filesystem::path& pluginPath) {
    detail::ModuleHandle moduleHandle = detail::openPluginModule(pluginPath);
    if (moduleHandle == nullptr) {
        return Error() << "Failed to open plugin " << pluginPath << ": " << detail::getLastModuleError();
    }

    const auto closeOnFailure = [&]() {
        detail::closePluginModule(moduleHandle);
        moduleHandle = nullptr;
    };

    void* symbol = detail::findPluginSymbol(moduleHandle, c_pluginProbeDescriptorSymbolName);
    if (symbol == nullptr) {
        const std::string symbolError = detail::getLastModuleError();
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " is missing symbol " << c_pluginProbeDescriptorSymbolName << ": "
                       << symbolError;
    }

    auto getProbeDescriptor = reinterpret_cast<GetPluginProbeDescriptorFunction>(symbol);
    PluginProbeDescriptor probeDescriptor;
    getProbeDescriptor(&probeDescriptor);

    if (probeDescriptor.m_magic != c_pluginProbeMagic) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " returned an invalid plugin probe descriptor magic value";
    }

    if (probeDescriptor.m_abiVersion != c_pluginProbeAbiVersion) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " uses unsupported plugin probe ABI version "
                       << probeDescriptor.m_abiVersion;
    }

    if (probeDescriptor.m_structSize < sizeof(PluginProbeDescriptor)) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " returned an undersized plugin probe descriptor";
    }

    if (probeDescriptor.writeBuildFingerprint == nullptr) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " returned an incomplete plugin probe descriptor";
    }

    const Version& hostVersion = Version::getCodebaseVersion();
    const Version pluginVersion{probeDescriptor.m_codebaseMajor, probeDescriptor.m_codebaseMinor, probeDescriptor.m_codebasePatch};
    if (!hostVersion.satisfies(pluginVersion)) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " requires BabelWires " << pluginVersion.toString()
                       << " but host is " << hostVersion.toString();
    }

    char hostFingerprint[babelwires::c_buildFingerprintBufferSize] = {};
#ifndef NDEBUG
    const std::size_t hostFingerprintSize =
#endif 
    writeMyBuildFingerprint(hostFingerprint, sizeof(hostFingerprint));
    assert(hostFingerprintSize > 0 && hostFingerprintSize < sizeof(hostFingerprint));

    char pluginFingerprint[babelwires::c_buildFingerprintBufferSize] = {};
    const std::size_t pluginFingerprintSize = probeDescriptor.writeBuildFingerprint(pluginFingerprint, sizeof(pluginFingerprint));
    if (pluginFingerprintSize == 0 || pluginFingerprintSize > sizeof(pluginFingerprint)) {
        closeOnFailure();
        return Error() << "Failed to read build fingerprint for plugin " << pluginPath;
    }

    const Result fingerprintResult = compareBuildFingerprints(pluginFingerprint, hostFingerprint);
    if (!fingerprintResult) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " is build-incompatible: " << fingerprintResult.error().toString();
    }

    auto parsedUuid = uuidFromProbeText(probeDescriptor.m_pluginUuidText);
    if (!parsedUuid) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " has an invalid UUID: " << parsedUuid.error().toString();
    }

    if (parsedUuid->isZero()) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " has an invalid UUID";
    }

    symbol = detail::findPluginSymbol(moduleHandle, c_pluginRegistrationSymbolName);
    if (symbol == nullptr) {
        const std::string symbolError = detail::getLastModuleError();
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " is missing symbol " << c_pluginRegistrationSymbolName << ": "
                       << symbolError;
    }

    auto registerPlugin = reinterpret_cast<RegisterPluginFunction>(symbol);
    if (registerPlugin == nullptr) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " has an invalid registration symbol";
    }

    return PluginHandle(moduleHandle, pluginVersion, *parsedUuid, registerPlugin, pluginPath);
}
