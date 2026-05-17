/**
 * BabelWires application main function.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BabelWiresExe/babelWiresOptions.hpp>

#include <BabelWiresQtUi/ValueModels/valueModelRegistry.hpp>
#include <BabelWiresQtUi/applicationIdentity.hpp>
#include <BabelWiresQtUi/uiMain.hpp>

#include <BabelWiresLib/FileFormat/sourceFileFormat.hpp>
#include <BabelWiresLib/FileFormat/targetFileFormat.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>
#include <BabelWiresLib/Processors/processorFactoryRegistry.hpp>
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/Serialization/projectSerialization.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/libRegistration.hpp>

#include <BaseLib/BuildCompatibility/buildInfo.hpp>
#include <BaseLib/Context/context.hpp>
#include <BaseLib/IO/fileDataSource.hpp>
#include <BaseLib/Identifiers/identifierRegistry.hpp>
#include <BaseLib/Log/debugLogger.hpp>
#include <BaseLib/Log/ostreamLogListener.hpp>
#include <BaseLib/Log/unifiedLog.hpp>
#include <BaseLib/PluginSupport/pluginManager.hpp>
#include <BaseLib/PluginSupport/pluginOperations.hpp>
#include <BaseLib/Random/randomService.hpp>
#include <BaseLib/Serialization/deserializationRegistry.hpp>
#include <BaseLib/libRegistration.hpp>

// "plugins"
#include <Domains/Music/MusicLib/libRegistration.hpp>
#include <Domains/Music/MusicLibUi/libRegistration.hpp>
#include <Domains/TestDomain/libRegistration.hpp>

#if !BABELWIRES_SHARED_BUILD
#include <Domains/Music/Plugins/TestPlugin/libRegistration.hpp>
#include <Smf/libRegistration.hpp>
#endif

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <tinyxml2.h>

using namespace babelwires;

/// Plugin files are expected to have this extension.
constexpr const char c_pluginFileExtension[] = ".bwplugin";
constexpr const char c_applicationTitle[] = "BabelWires";
constexpr const char c_projectExtension[] = ".babelwires";

int main(int argc, char* argv[]) {
    auto options = ProgramOptions::parse(argc, argv);
    if (!options) {
        std::cerr << options.error().toString() << std::endl;
        writeUsage(argv[0], std::cerr);
        return EXIT_FAILURE;
    }

    if (options->m_mode == ProgramOptions::MODE_PRINT_HELP) {
        writeHelp(argv[0], std::cout);
        return EXIT_SUCCESS;
    }

    babelwires::IdentifierRegistryScope identifierRegistry;

    babelwires::OStreamLogListener::Features features = babelwires::OStreamLogListener::Features::none;
#ifndef NDEBUG
    features = features | babelwires::OStreamLogListener::Features::logDebugMessages |
               babelwires::OStreamLogListener::Features::timestamp;
#endif
    babelwires::UnifiedLog log;
    babelwires::DebugLogger::swapGlobalDebugLogger(&log);
    babelwires::OStreamLogListener logToCout(std::cout, log, features);

    logDebug() << "Build info:\n" << getBuildFingerprint();

#if BABELWIRES_SHARED_BUILD
    // Because the plugin manager keeps code in memory, it needs a lifetime that encompasses other services.
    babelwires::PluginManager pluginManager(c_pluginFileExtension);
#endif

    SourceFileFormatRegistry sourceFileFormatReg;
    TargetFileFormatRegistry targetFileFormatReg;
    ProcessorFactoryRegistry processorReg;
    babelwires::DeserializationRegistry deserializationRegistry;
    babelwires::ValueModelRegistry valueModelRegistry;
    babelwires::TypeSystem typeSystem;

    const unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    babelwires::logDebug() << "The random seed was " << seed;
    babelwires::RandomService randomService(seed);

    babelwires::Context context;

    babelwires::ApplicationIdentity applicationIdentity;
    applicationIdentity.m_applicationTitle = c_applicationTitle;
    applicationIdentity.m_projectExtension = c_projectExtension;

    context.registerService<babelwires::DeserializationRegistry>(deserializationRegistry);
    context.registerService<babelwires::RandomService>(randomService);
    context.registerService<babelwires::SourceFileFormatRegistry>(sourceFileFormatReg);
    context.registerService<babelwires::TargetFileFormatRegistry>(targetFileFormatReg);
    context.registerService<babelwires::ProcessorFactoryRegistry>(processorReg);
    context.registerService<babelwires::TypeSystem>(typeSystem);
    context.registerService<babelwires::ValueModelRegistry>(valueModelRegistry);
    context.registerService<babelwires::ApplicationIdentity>(applicationIdentity);

    // register factories, etc.
    babelwires::baseLib::registerLib(context);
    babelwires::registerLib(context);
    bw_music::registerLib(context);
    bw_musicUi::registerLib(context);

#if BABELWIRES_SHARED_BUILD
    // TODO Stop the test plugin loading automatically.
    const unsigned int loadedPlugins = pluginManager.loadAllPlugins(BABELWIRES_DEFAULT_PLUGIN_DIR, context, log);
    babelwires::logDebug() << "Loaded " << loadedPlugins << " plugin(s) from " << BABELWIRES_DEFAULT_PLUGIN_DIR;
#else
    ASSERT_NO_ERROR(smf::registerLib(context, log));
    // ASSERT_NO_ERROR(bw_music_testplugin::registerLib(context, log));
#endif

     testDomain::registerLib(context);

    if (options->m_mode == ProgramOptions::MODE_RUN_PROJECT) {
        Project project(context, log);
        ResultT<ProjectData> projectDataResult =
            ProjectSerialization::loadFromFile(options->m_inputFileName.c_str(), context, log);
        if (!projectDataResult) {
            std::cerr << "Error loading project: " << projectDataResult.error().toString() << std::endl;
            return EXIT_FAILURE;
        }
        project.setProjectData(std::move(*projectDataResult));
        project.tryToSaveAllTargets();
        return EXIT_SUCCESS;
    } else {
        Ui ui(argc, argv, context, log);
        if (argc > 1) {
            std::cerr << "Unexpected arguments";
            writeUsage(argv[0], std::cerr);
            return EXIT_FAILURE;
        }
        ui.runMainLoop();
    }
    return EXIT_FAILURE;
}
