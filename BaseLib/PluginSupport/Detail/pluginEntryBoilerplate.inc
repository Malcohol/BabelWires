/**
 * Common plugin entry-point boilerplate.
 *
 * Expects:
 *   - PLUGIN_REGISTRATION_FUNCTION to name the plugin registration function.
 *   - PLUGIN_UUID to contain a hardcoded UUID string.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#ifndef PLUGIN_REGISTRATION_FUNCTION
    #error "PLUGIN_REGISTRATION_FUNCTION must be defined before including pluginEntryDefinition.inc"
#endif

#ifndef PLUGIN_UUID
    #error "PLUGIN_UUID must be defined before including pluginEntryDefinition.inc"
#endif

#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>
#include <BaseLib/PluginSupport/pluginDescriptor.hpp>
#include <BaseLib/Version/codebaseVersion_gen.hpp>

#include <cstdint>

static_assert(
    sizeof(PLUGIN_UUID) == (babelwires::c_pluginUuidTextSize + 1),
    "PLUGIN_UUID must be a 36-character canonical UUID string literal");

namespace {
    std::size_t pluginLocalBuildFingerprint(char* buffer, std::size_t bufferSize) {
        return babelwires::writeMyBuildFingerprint(buffer, bufferSize);
    }

    void writePluginUuidText(std::uint8_t* outUuidText) {
        const char* uuidText = PLUGIN_UUID;
        for (std::size_t index = 0; index < babelwires::c_pluginUuidTextSize; ++index) {
            outUuidText[index] = static_cast<std::uint8_t>(uuidText[index]);
        }
    }
} // namespace

extern "C" BABELWIRES_PLUGIN_EXPORT int BABELWIRES_PLUGIN_CALL babelwires_getPluginProbeDescriptor(
    babelwires::PluginProbeDescriptor* inOut) {
    if (inOut == nullptr) {
        return -1;
    }
    if (inOut->m_magicNumberInOut != babelwires::c_pluginProbeHostMagicNumber) {
        return -1;
    }
    if (inOut->m_structSizeInOut < sizeof(babelwires::PluginProbeDescriptor)) {
        return -1;
    }
    // No test on c_pluginProbeAbiVersion in version 1.
    inOut->m_magicNumberInOut = babelwires::c_pluginProbePluginMagicNumber;
    inOut->m_probeVersionInOut = babelwires::c_pluginProbeAbiVersion;
    inOut->m_structSizeInOut = sizeof(babelwires::PluginProbeDescriptor);
    inOut->m_codebaseMajor = babelwires::c_myCodebaseVersion.m_major;
    inOut->m_codebaseMinor = babelwires::c_myCodebaseVersion.m_minor;
    inOut->m_codebasePatch = babelwires::c_myCodebaseVersion.m_patch;
    writePluginUuidText(inOut->m_pluginUuidText);
    // Important: Don't take the address of BaseLib's writeMyBuildFingerprint, but instead the address of the local
    // wrapper.
    inOut->writeBuildFingerprint = &pluginLocalBuildFingerprint;
    return 0;
}

extern "C" BABELWIRES_PLUGIN_EXPORT babelwires::Result BABELWIRES_PLUGIN_CALL babelwires_registerPlugin(
    babelwires::Context& context,
    babelwires::UserAdvisoryLogger& userLogger) {
    return PLUGIN_REGISTRATION_FUNCTION(context, userLogger);
}

