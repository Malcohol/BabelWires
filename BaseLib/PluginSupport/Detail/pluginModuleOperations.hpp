/**
 * Wrapper functions for platform-specific dynamic module operations.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <filesystem>
#include <string>

namespace babelwires::detail {

    using ModuleHandle = void*;

    ModuleHandle openPluginModule(const std::filesystem::path& pluginPath);
    void closePluginModule(ModuleHandle moduleHandle);
    void* findPluginSymbol(ModuleHandle moduleHandle, const char* symbolName);
    std::string getLastModuleError();

} // namespace babelwires::detail
