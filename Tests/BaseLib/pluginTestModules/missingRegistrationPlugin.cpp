#include <BaseLib/PluginSupport/pluginDescriptor.hpp>

#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>
#include <BaseLib/Version/codebaseVersion_gen.hpp>

namespace {
    std::size_t BABELWIRES_PLUGIN_CALL testFingerprint(char* buffer, std::size_t bufferSize) {
        return babelwires::writeMyBuildFingerprint(buffer, bufferSize);
    }

    void writePluginUuidText(std::uint8_t* outUuidText, const char* uuidText) {
        for (std::size_t index = 0; index < babelwires::c_pluginUuidTextSize; ++index) {
            outUuidText[index] = static_cast<std::uint8_t>(uuidText[index]);
        }
    }
}

extern "C" BABELWIRES_PLUGIN_EXPORT int BABELWIRES_PLUGIN_CALL babelwires_getPluginProbeDescriptor(
    babelwires::PluginProbeDescriptor* out) {
    if (out == nullptr) {
        return -1;
    }

    out->m_magicNumberInOut = babelwires::c_pluginProbePluginMagicNumber;
    out->m_probeVersionInOut = babelwires::c_pluginProbeAbiVersion;
    out->m_structSizeInOut = sizeof(babelwires::PluginProbeDescriptor);
    out->m_codebaseMajor = babelwires::c_myCodebaseVersion.m_major;
    out->m_codebaseMinor = babelwires::c_myCodebaseVersion.m_minor;
    out->m_codebasePatch = babelwires::c_myCodebaseVersion.m_patch;
    writePluginUuidText(out->m_pluginUuidText, "11111111-2222-3333-4444-555555555555");
    out->writeBuildFingerprint = &testFingerprint;
    return 0;
}