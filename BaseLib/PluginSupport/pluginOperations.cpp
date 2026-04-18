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
#include <BaseLib/Context/context.hpp>
#include <BaseLib/Log/userLogger.hpp>
#include <BaseLib/PluginSupport/pluginDescriptor.hpp>
#include <BaseLib/PluginSupport/detail/pluginModuleOperations.hpp>
#include <BaseLib/Result/resultDSL.hpp>
#include <BaseLib/Version/version.hpp>

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace {
    std::vector<babelwires::detail::ModuleHandle>& loadedPluginModules() {
        static std::vector<babelwires::detail::ModuleHandle> modules;
        return modules;
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

babelwires::ResultT<babelwires::PluginHandle> babelwires::validatePlugin(const std::filesystem::path& pluginPath) {
    detail::ModuleHandle moduleHandle = detail::openPluginModule(pluginPath);
    if (moduleHandle == nullptr) {
        return Error() << "Failed to open plugin " << pluginPath << ": " << detail::getLastModuleError();
    }

    const auto closeOnFailure = [&]() {
        detail::closePluginModule(moduleHandle);
        moduleHandle = nullptr;
    };

    void* symbol = detail::findPluginSymbol(moduleHandle, c_pluginDescriptorSymbolName);
    if (symbol == nullptr) {
        const std::string symbolError = detail::getLastModuleError();
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " is missing symbol " << c_pluginDescriptorSymbolName << ": "
                       << symbolError;
    }

    auto getDescriptor = reinterpret_cast<GetPluginDescriptorFunction>(symbol);
    PluginDescriptor descriptor;
    getDescriptor(&descriptor);

    if ((descriptor.getBuildFingerprint == nullptr) || (descriptor.registerPlugin == nullptr)) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " returned an incomplete plugin descriptor";
    }

    const Version& hostVersion = Version::getCodebaseVersion();
    if (!hostVersion.satisfies(descriptor.m_codebaseVersion)) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " requires BabelWires " << descriptor.m_codebaseVersion.toString()
                       << " but host is " << hostVersion.toString();
    }

    char hostFingerprint[babelwires::c_buildFingerprintBufferSize] = {};
#ifndef NDEBUG
    const std::size_t hostFingerprintSize =
#endif 
    writeMyBuildFingerprint(hostFingerprint, sizeof(hostFingerprint));
    assert(hostFingerprintSize > 0 && hostFingerprintSize < sizeof(hostFingerprint));

    char pluginFingerprint[babelwires::c_buildFingerprintBufferSize] = {};
    const std::size_t pluginFingerprintSize = descriptor.getBuildFingerprint(pluginFingerprint, sizeof(pluginFingerprint));
    if (pluginFingerprintSize == 0 || pluginFingerprintSize > sizeof(pluginFingerprint)) {
        closeOnFailure();
        return Error() << "Failed to read build fingerprint for plugin " << pluginPath;
    }

    const Result fingerprintResult = compareBuildFingerprints(pluginFingerprint, hostFingerprint);
    if (!fingerprintResult) {
        closeOnFailure();
        return Error() << "Plugin " << pluginPath << " is build-incompatible: " << fingerprintResult.error().toString();
    }

    return PluginHandle(moduleHandle, descriptor, pluginPath);
}

babelwires::Result babelwires::loadPlugin(PluginHandle&& handle, Context& context) {
    if (!handle) {
        return Error() << "Cannot load an empty plugin handle";
    }

    const PluginDescriptor descriptor = handle.getDescriptor();
    if (descriptor.registerPlugin == nullptr) {
        return Error() << "Plugin " << handle.getPluginPath() << " has no registerPlugin function";
    }

    try {
        descriptor.registerPlugin(context);
    } catch (const std::exception& ex) {
        return Error() << "Plugin " << handle.getPluginPath() << " threw during registration: " << ex.what();
    } catch (...) {
        return Error() << "Plugin " << handle.getPluginPath() << " threw during registration";
    }

    loadedPluginModules().push_back(handle.releaseModuleHandle());
    return Result{};
}

unsigned int babelwires::loadAllPlugins(const std::filesystem::path& pluginDir, Context& context, UserLogger& userLogger) {
    ResultT<std::vector<std::filesystem::path>> discovered = discoverPlugins(pluginDir, userLogger);
    if (!discovered) {
        userLogger.logWarning() << discovered.error().toString();
        return 0;
    }

    unsigned int loadedCount = 0;
    for (const auto& pluginPath : *discovered) {
        ResultT<PluginHandle> validated = validatePlugin(pluginPath);
        if (!validated) {
            userLogger.logWarning() << validated.error().toString();
            continue;
        }

        Result loaded = loadPlugin(std::move(*validated), context);
        if (!loaded) {
            userLogger.logWarning() << loaded.error().toString();
            continue;
        }

        ++loadedCount;
    }

    return loadedCount;
}
