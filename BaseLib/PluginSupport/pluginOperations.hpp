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
    class UserLogger;

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
    /// The caller must keep the PluginHandle alive; dropping it calls dlclose.
    BASELIB_API ResultT<PluginHandle> validatePlugin(const std::filesystem::path& pluginPath);

    /// Register a previously validated plugin into the Context.
    /// On success, the PluginHandle is consumed and the plugin stays loaded for the process lifetime.
    /// On failure (e.g. exception in registerPlugin), returns an error.
    BASELIB_API Result loadPlugin(PluginHandle&& handle, Context& context);

    /// Discover, validate, and load all plugins from a directory.
    /// This is the main entry point for startup. It never "fails" — individual plugin
    /// failures are logged as warnings to the provided UserLogger, and the function
    /// continues to the next plugin. Returns the number of plugins successfully loaded.
    BASELIB_API unsigned int loadAllPlugins(const std::filesystem::path& pluginDir,
                                            Context& context,
                                            UserLogger& userLogger);

} // namespace babelwires
