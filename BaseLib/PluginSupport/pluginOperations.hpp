/**
 * Plugin discovery, validation and loading operations.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/PluginSupport/pluginHandle.hpp>
#include <BaseLib/Result/result.hpp>
#include <BaseLib/baseLibExport.hpp>

#include <filesystem>
#include <vector>

namespace babelwires {

    class Context;
    struct UserLogger;

    /// Plugin files are expected to have this extension.
    constexpr const char c_pluginFileExtension[] = ".bwplugin";

    /// Scan a directory tree for plugin files.
    /// Returns the list of filesystem paths found.
    /// Symlinks are followed, but the recursion depth is limited.
    /// Fails if the directory does not exist or is not readable,
    /// but warns (without failing) if some entries cannot be accessed due to permissions.
    BASELIB_API ResultT<std::vector<std::filesystem::path>> discoverPlugins(const std::filesystem::path& pluginDir, UserLogger& userLogger);

    /// Open a plugin file and validate compatibility without registering anything.
    /// On success, returns an opaque PluginHandle.
    /// On failure, returns an error describing the problem (bad file, missing symbol,
    /// version mismatch, fingerprint mismatch, etc.).
    /// The caller must keep the PluginHandle alive.
    BASELIB_API ResultT<PluginHandle> openPlugin(const std::filesystem::path& pluginPath);

} // namespace babelwires
