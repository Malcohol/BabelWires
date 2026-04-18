/**
 * Wrapper functions for platform-specific dynamic module operations.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/PluginSupport/detail/pluginModuleOperations.hpp>

#if defined(_WIN32) || defined(_WIN64)
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

babelwires::detail::ModuleHandle babelwires::detail::openPluginModule(const std::filesystem::path& pluginPath) {
#if defined(_WIN32) || defined(_WIN64)
    return reinterpret_cast<ModuleHandle>(LoadLibraryW(pluginPath.c_str()));
#else
    return dlopen(pluginPath.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
}

void babelwires::detail::closePluginModule(ModuleHandle moduleHandle) {
    if (moduleHandle == nullptr) {
        return;
    }

#if defined(_WIN32) || defined(_WIN64)
    FreeLibrary(reinterpret_cast<HMODULE>(moduleHandle));
#else
    dlclose(moduleHandle);
#endif
}

void* babelwires::detail::findPluginSymbol(ModuleHandle moduleHandle, const char* symbolName) {
#if defined(_WIN32) || defined(_WIN64)
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(moduleHandle), symbolName));
#else
    return dlsym(moduleHandle, symbolName);
#endif
}

std::string babelwires::detail::getLastModuleError() {
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
