/**
 * Owning handle for a validated plugin module.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/PluginSupport/pluginHandle.hpp>

#include <BaseLib/PluginSupport/Detail/pluginModuleOperations.hpp>

#include <cassert>
#include <utility>

babelwires::PluginHandle::PluginHandle(void* moduleHandle, PluginDescriptor descriptor,
                                       std::filesystem::path pluginPath)
    : m_moduleHandle(moduleHandle)
    , m_descriptor(descriptor)
    , m_pluginPath(std::move(pluginPath)) {
    assert((m_moduleHandle != nullptr) && "PluginHandle should only be constructed with a valid module handle");
}

babelwires::PluginHandle::~PluginHandle() {
    detail::closePluginModule(m_moduleHandle);
}

babelwires::PluginHandle::PluginHandle(PluginHandle&& other) noexcept
    : m_moduleHandle(other.m_moduleHandle)
    , m_descriptor(other.m_descriptor)
    , m_pluginPath(std::move(other.m_pluginPath)) {
    other.m_moduleHandle = nullptr;
}

babelwires::PluginHandle& babelwires::PluginHandle::operator=(PluginHandle&& other) noexcept {
    if (this != &other) {
        detail::closePluginModule(m_moduleHandle);
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
