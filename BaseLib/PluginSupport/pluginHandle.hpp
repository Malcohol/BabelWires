/**
 * Owning handle for a validated plugin module.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/PluginSupport/pluginDescriptor.hpp>
#include <BaseLib/Version/version.hpp>
#include <BaseLib/uuid.hpp>
#include <BaseLib/baseLibExport.hpp>

#include <filesystem>

namespace babelwires {

    /// Owning handle for a validated plugin module.
    class BASELIB_API PluginHandle {
      public:
        PluginHandle(void* moduleHandle,
                     Version pluginVersion,
                     Uuid pluginUuid,
                     RegisterPluginFunction registerPlugin,
                     std::filesystem::path pluginPath);
        ~PluginHandle();

        PluginHandle(const PluginHandle&) = delete;
        PluginHandle& operator=(const PluginHandle&) = delete;

        PluginHandle(PluginHandle&& other) noexcept;
        PluginHandle& operator=(PluginHandle&& other) noexcept;

        const Version& getPluginVersion() const;
        const Uuid& getPluginUuid() const;
        RegisterPluginFunction getRegisterPluginFunction() const;

        /// Get the path to the location of the plugin file.
        const std::filesystem::path& getPluginPath() const;

      private:
        /// Low-level module handle.
        void* m_moduleHandle;
        Version m_pluginVersion{};
        Uuid m_pluginUuid{};
        RegisterPluginFunction m_registerPlugin = nullptr;
        std::filesystem::path m_pluginPath;
    };

} // namespace babelwires
