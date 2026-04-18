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

#include <utility>

babelwires::Result babelwires::PluginManager::loadPlugin(PluginHandle&& handle, Context& context, UserLogger& userLogger) {
    const PluginDescriptor descriptor = handle.getDescriptor();
    if (descriptor.registerPlugin == nullptr) {
        return Error() << "Plugin " << handle.getPluginPath() << " has no registerPlugin function";
    }

    const Result registrationResult = descriptor.registerPlugin(context, userLogger);
    if (!registrationResult) {
        return Error() << "Plugin " << handle.getPluginPath() << " failed during registration: "
                       << registrationResult.error().toString();
    }

    m_loadedPlugins.push_back(std::move(handle));
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

        Result loaded = loadPlugin(std::move(*validated), context, userLogger);
        if (!loaded) {
            userLogger.logWarning() << loaded.error().toString();
            continue;
        }

        ++loadedCount;
    }

    return loadedCount;
}
