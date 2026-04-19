/**
 * Stateful plugin loading manager.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/PluginSupport/pluginManager.hpp>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/Log/userLogger.hpp>
#include <BaseLib/PluginSupport/pluginDescriptor.hpp>
#include <BaseLib/PluginSupport/pluginOperations.hpp>
#include <BaseLib/Result/resultDSL.hpp>

#include <algorithm>
#include <utility>

bool babelwires::PluginManager::isPluginLoaded(const Uuid& pluginUuid) const {
    return std::any_of(m_loadedPlugins.begin(),
                       m_loadedPlugins.end(),
                       [&](const PluginHandle& loaded) { return loaded.getPluginUuid() == pluginUuid; });
}

babelwires::Result babelwires::PluginManager::loadPlugin(PluginHandle&& handle, Context& context, UserLogger& userLogger) {
    const RegisterPluginFunction registerPlugin = handle.getRegisterPluginFunction();
    if (registerPlugin == nullptr) {
        return Error() << "Plugin " << handle.getPluginPath() << " has no registerPlugin function";
    }

    if (isPluginLoaded(handle.getPluginUuid())) {
        return Error() << "Plugin " << handle.getPluginPath() << " duplicates an already loaded plugin UUID "
                       << handle.getPluginUuid();
    }

    userLogger.logInfo() << "Loading plugin " << handle.getPluginPath();
    const Result registrationResult = registerPlugin(context, userLogger);
    if (!registrationResult) {
        return Error() << "Plugin " << handle.getPluginPath() << " failed during registration: "
                       << registrationResult.error().toString();
    }

    m_loadedPlugins.push_back(std::move(handle));
    userLogger.logInfo() << "Successfully loaded plugin " << m_loadedPlugins.back().getPluginPath();
    return Result{};
}

unsigned int babelwires::PluginManager::loadAllPlugins(const std::filesystem::path& pluginDir,
                                                        Context& context,
                                                        UserLogger& userLogger) {
    ResultT<std::vector<std::filesystem::path>> discovered = discoverPlugins(pluginDir, userLogger);
    if (!discovered) {
        userLogger.logWarning() << discovered.error().toString();
        return 0;
    }

    unsigned int loadedCount = 0;
    for (const auto& pluginPath : *discovered) {
        ResultT<PluginHandle> validated = openPlugin(pluginPath);
        if (!validated) {
            userLogger.logWarning() << validated.error().toString();
            continue;
        }

        if (isPluginLoaded(validated->getPluginUuid())) {
            userLogger.logWarning() << "Skipping duplicate plugin " << pluginPath << " with UUID " << validated->getPluginUuid();
            continue;
        }

        Result loaded = loadPlugin(std::move(*validated), context, userLogger);
        if (!loaded) {
            userLogger.logWarning() << loaded.error().toString();
            continue;
        }

        ++loadedCount;
    }

    return loadedCount;
}
