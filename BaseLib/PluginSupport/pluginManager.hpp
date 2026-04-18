/**
 * Stateful plugin loading manager.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/PluginSupport/pluginHandle.hpp>
#include <BaseLib/Result/result.hpp>
#include <BaseLib/uuid.hpp>
#include <BaseLib/baseLibExport.hpp>

#include <filesystem>
#include <vector>

namespace babelwires {

    class Context;
    struct UserLogger;

    /// Manages registration of validated plugins and owns loaded plugin handles.
    /// Note: The plugin manager keeps code in memory, so it needs a lifetime that encompasses other services.
    class BASELIB_API PluginManager {
      public:
        /// Returns true if a plugin with this UUID is already loaded.
        bool isPluginLoaded(const Uuid& pluginUuid) const;

        /// Register a previously validated plugin into the Context.
        /// On success, the PluginHandle is consumed and the plugin stays loaded for this manager's lifetime.
        Result loadPlugin(PluginHandle&& handle, Context& context, UserLogger& userLogger);

        /// Discover, validate, and load all plugins from a directory.
        /// Individual plugin failures are logged as warnings and loading continues.
        /// Returns the number of plugins successfully loaded.
        unsigned int loadAllPlugins(const std::filesystem::path& pluginDir, Context& context, UserLogger& userLogger);

      private:
        std::vector<PluginHandle> m_loadedPlugins;
    };

} // namespace babelwires
