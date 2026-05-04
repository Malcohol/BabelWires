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

babelwires::PluginHandle::PluginHandle(void* moduleHandle,
                                       Version pluginVersion,
                                       Uuid pluginUuid,
                                       RegisterPluginFunction registerPlugin,
                                       std::filesystem::path pluginPath)
    : m_moduleHandle(moduleHandle)
    , m_pluginVersion(pluginVersion)
    , m_pluginUuid(std::move(pluginUuid))
    , m_registerPlugin(registerPlugin)
    , m_pluginPath(std::move(pluginPath)) {
    assert((m_moduleHandle != nullptr) && "PluginHandle should only be constructed with a valid module handle");
}

babelwires::PluginHandle::~PluginHandle() {
    detail::closePluginModule(m_moduleHandle);
}

babelwires::PluginHandle::PluginHandle(PluginHandle&& other) noexcept
    : m_moduleHandle(other.m_moduleHandle)
    , m_pluginVersion(other.m_pluginVersion)
    , m_pluginUuid(std::move(other.m_pluginUuid))
    , m_registerPlugin(other.m_registerPlugin)
    , m_pluginPath(std::move(other.m_pluginPath)) {
    other.m_moduleHandle = nullptr;
    other.m_registerPlugin = nullptr;
}

babelwires::PluginHandle& babelwires::PluginHandle::operator=(PluginHandle&& other) noexcept {
    if (this != &other) {
        detail::closePluginModule(m_moduleHandle);
        m_moduleHandle = other.m_moduleHandle;
        m_pluginVersion = other.m_pluginVersion;
        m_pluginUuid = std::move(other.m_pluginUuid);
        m_registerPlugin = other.m_registerPlugin;
        m_pluginPath = std::move(other.m_pluginPath);
        other.m_moduleHandle = nullptr;
        other.m_registerPlugin = nullptr;
    }
    return *this;
}

const babelwires::Version& babelwires::PluginHandle::getPluginVersion() const {
    return m_pluginVersion;
}

const babelwires::Uuid& babelwires::PluginHandle::getPluginUuid() const {
    return m_pluginUuid;
}

babelwires::RegisterPluginFunction babelwires::PluginHandle::getRegisterPluginFunction() const {
    return m_registerPlugin;
}

const std::filesystem::path& babelwires::PluginHandle::getPluginPath() const {
    return m_pluginPath;
}
