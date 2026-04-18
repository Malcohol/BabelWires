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

    class BASELIB_API PluginHandle {
      public:
        PluginHandle() = default;
        PluginHandle(void* moduleHandle, PluginDescriptor descriptor, std::filesystem::path pluginPath);
        ~PluginHandle();

        PluginHandle(const PluginHandle&) = delete;
        PluginHandle& operator=(const PluginHandle&) = delete;

        PluginHandle(PluginHandle&& other) noexcept;
        PluginHandle& operator=(PluginHandle&& other) noexcept;

        const PluginDescriptor& getDescriptor() const;
        const std::filesystem::path& getPluginPath() const;

        void* releaseModuleHandle();

        explicit operator bool() const;

      private:
        void* m_moduleHandle = nullptr;
        PluginDescriptor m_descriptor{};
        std::filesystem::path m_pluginPath;
    };

} // namespace babelwires
