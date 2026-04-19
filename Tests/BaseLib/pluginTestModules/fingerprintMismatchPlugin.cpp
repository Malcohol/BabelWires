#include <BaseLib/PluginSupport/pluginDescriptor.hpp>

#include <BaseLib/Version/codebaseVersion_gen.hpp>

#include <cstring>

namespace {
    std::size_t BABELWIRES_PLUGIN_CALL mismatchedFingerprint(char* buffer, std::size_t bufferSize) {
        static constexpr const char* c_text = "schema_version=1\ncompiler=test\n";
        if ((buffer == nullptr) || (bufferSize == 0)) {
            return 0;
        }

        const std::size_t sourceLength = std::strlen(c_text);
        const std::size_t bytesToCopy = (sourceLength < (bufferSize - 1)) ? sourceLength : (bufferSize - 1);
        std::memcpy(buffer, c_text, bytesToCopy);
        buffer[bytesToCopy] = 0;
        return bytesToCopy + 1;
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
    out->writeBuildFingerprint = &mismatchedFingerprint;
    return 0;
}

extern "C" BABELWIRES_PLUGIN_EXPORT babelwires::Result BABELWIRES_PLUGIN_CALL babelwires_registerPlugin(
    babelwires::Context&, babelwires::UserAdvisoryLogger&) {
    return babelwires::Result{};
}