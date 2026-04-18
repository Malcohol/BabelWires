/**
 * Owning handle for a validated plugin module.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/PluginSupport/pluginDescriptor.hpp>
#include <BaseLib/baseLibExport.hpp>

#include <filesystem>

namespace babelwires {

    /// Owning handle for a validated plugin module.
    class BASELIB_API PluginHandle {
      public:
        PluginHandle(void* moduleHandle, PluginDescriptor descriptor, std::filesystem::path pluginPath);
        ~PluginHandle();

        PluginHandle(const PluginHandle&) = delete;
        PluginHandle& operator=(const PluginHandle&) = delete;

        PluginHandle(PluginHandle&& other) noexcept;
        PluginHandle& operator=(PluginHandle&& other) noexcept;

        /// Get the plugin's descriptor, which provides information about the plugin and pointers to its functions.
        const PluginDescriptor& getDescriptor() const;

        /// Get the path to the location of the plugin file.
        const std::filesystem::path& getPluginPath() const;

      private:
        /// Low-level module handle.
        void* m_moduleHandle;
        PluginDescriptor m_descriptor{};
        std::filesystem::path m_pluginPath;
    };

} // namespace babelwires
