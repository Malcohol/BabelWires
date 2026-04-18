/**
 * Common plugin entry-point boilerplate.
 *
 * Expects:
 *   - PLUGIN_REGISTRATION_FUNCTION to name the plugin registration function.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#ifndef PLUGIN_REGISTRATION_FUNCTION
    #error "PLUGIN_REGISTRATION_FUNCTION must be defined before including pluginEntryDefinition.inc"
#endif

#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>
#include <BaseLib/PluginSupport/pluginDescriptor.hpp>
#include <BaseLib/Version/codebaseVersion_gen.hpp>

namespace {
    std::size_t pluginLocalBuildFingerprint(char* buffer, std::size_t bufferSize) {
        return babelwires::writeMyBuildFingerprint(buffer, bufferSize);
    }
} // namespace

extern "C" BABELWIRES_PLUGIN_EXPORT void babelwires_getPluginDescriptor(babelwires::PluginDescriptor* out) {
    out->m_codebaseVersion = babelwires::c_myCodebaseVersion;
    // Important: Don't take the address of BaseLib's writeMyBuildFingerprint, but instead the address of the local
    // wrapper.
    out->getBuildFingerprint = &pluginLocalBuildFingerprint;
    out->registerPlugin = &PLUGIN_REGISTRATION_FUNCTION;
}

