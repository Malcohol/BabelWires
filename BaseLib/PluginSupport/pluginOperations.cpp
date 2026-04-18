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
#include <BaseLib/Result/resultDSL.hpp>
#include <BaseLib/Version/version.hpp>

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

namespace {

    std::string getLastError() {
#if defined(_WIN32) || defined(_WIN64)
        const DWORD errorCode = GetLastError();
        if (errorCode == 0) {
            return "Unknown error";
        }

        LPSTR messageBuffer = nullptr;
        const DWORD messageLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                                   nullptr,
                                                   errorCode,
                                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                                   reinterpret_cast<LPSTR>(&messageBuffer),
                                                   0,
                                                   nullptr);
        std::string message = (messageLength > 0 && messageBuffer != nullptr) ? messageBuffer : "Unknown error";
        if (messageBuffer != nullptr) {
            LocalFree(messageBuffer);
        }
        return message;
#else
        const char* errorText = dlerror();
        return (errorText != nullptr) ? std::string(errorText) : std::string("Unknown error");
#endif
    }

    using ModuleHandle = void*;

    ModuleHandle openPluginModule(const std::filesystem::path& pluginPath) {
#if defined(_WIN32) || defined(_WIN64)
        return reinterpret_cast<ModuleHandle>(LoadLibraryW(pluginPath.c_str()));
#else
        return dlopen(pluginPath.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
    }

    void closePluginModule(ModuleHandle moduleHandle) {
        if (moduleHandle == nullptr) {
            return;
        }
#if defined(_WIN32) || defined(_WIN64)
        FreeLibrary(reinterpret_cast<HMODULE>(moduleHandle));
#else
        dlclose(moduleHandle);
#endif
    }

    void* findPluginSymbol(ModuleHandle moduleHandle, const char* symbolName) {
#if defined(_WIN32) || defined(_WIN64)
        return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(moduleHandle), symbolName));
#else
        return dlsym(moduleHandle, symbolName);
#endif
    }

    std::vector<ModuleHandle>& loadedPluginModules() {
        static std::vector<ModuleHandle> modules;
        return modules;
    }

} // namespace

babelwires::PluginHandle::PluginHandle(void* moduleHandle, PluginDescriptor descriptor, std::filesystem::path pluginPath)
    : m_moduleHandle(moduleHandle)
    , m_descriptor(descriptor)
    , m_pluginPath(std::move(pluginPath)) {}

babelwires::PluginHandle::~PluginHandle() {
    closePluginModule(m_moduleHandle);
}

babelwires::PluginHandle::PluginHandle(PluginHandle&& other) noexcept
    : m_moduleHandle(other.m_moduleHandle)
    , m_descriptor(other.m_descriptor)
    , m_pluginPath(std::move(other.m_pluginPath)) {
    other.m_moduleHandle = nullptr;
}

babelwires::PluginHandle& babelwires::PluginHandle::operator=(PluginHandle&& other) noexcept {
    if (this != &other) {
        closePluginModule(m_moduleHandle);
        m_moduleHandle = other.m_moduleHandle;
        m_descriptor = other.m_descriptor;
        m_pluginPath = std::move(other.m_pluginPath);
        other.m_moduleHandle = nullptr;
    }
    return *this;
}

const babelwires::PluginDescriptor& babelwires::PluginHandle::getDescriptor() const {
    return m_descriptor;
}

const std::filesystem::path& babelwires::PluginHandle::getPluginPath() const {
    return m_pluginPath;
}

void* babelwires::PluginHandle::releaseModuleHandle() {
    void* handle = m_moduleHandle;
    m_moduleHandle = nullptr;
    return handle;
}

babelwires::PluginHandle::operator bool() const {
    return m_moduleHandle != nullptr;
}

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
    ModuleHandle moduleHandle = openPluginModule(pluginPath);
    if (moduleHandle == nullptr) {
        return Error() << "Failed to open plugin " << pluginPath << ": " << getLastError();
    }

    const auto closeOnFailure = [&]() {
        closePluginModule(moduleHandle);
        moduleHandle = nullptr;
    };

    void* symbol = findPluginSymbol(moduleHandle, c_pluginDescriptorSymbolName);
    if (symbol == nullptr) {
        const std::string symbolError = getLastError();
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
