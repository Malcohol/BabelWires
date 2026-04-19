#include <gtest/gtest.h>

#include <BaseLib/PluginSupport/pluginOperations.hpp>
#include <BaseLib/Version/version.hpp>

#include <string>

namespace {
    std::filesystem::path goodPluginPath() {
        return BASELIB_TEST_PLUGIN_GOOD_PATH;
    }

    std::filesystem::path badMagicPluginPath() {
        return BASELIB_TEST_PLUGIN_BAD_MAGIC_PATH;
    }

    std::filesystem::path fingerprintMismatchPluginPath() {
        return BASELIB_TEST_PLUGIN_FINGERPRINT_MISMATCH_PATH;
    }

    std::filesystem::path missingProbePluginPath() {
        return BASELIB_TEST_PLUGIN_MISSING_PROBE_PATH;
    }

    std::filesystem::path missingRegistrationPluginPath() {
        return BASELIB_TEST_PLUGIN_MISSING_REGISTRATION_PATH;
    }
} // namespace

TEST(PluginSupportTest, openPluginReadsValidatedProbeMetadata) {
    auto opened = babelwires::openPlugin(goodPluginPath());

    ASSERT_TRUE(opened) << opened.error().toString();
    EXPECT_EQ(opened->getPluginVersion().toString(), babelwires::Version::getCodebaseVersion().toString());
    EXPECT_EQ(opened->getPluginUuid().serializeToString(), "11111111-2222-3333-4444-555555555555");
    EXPECT_NE(opened->getRegisterPluginFunction(), nullptr);
}

TEST(PluginSupportTest, openPluginRejectsMissingProbeSymbol) {
    auto opened = babelwires::openPlugin(missingProbePluginPath());

    ASSERT_FALSE(opened);
    EXPECT_NE(opened.error().toString().find(babelwires::c_pluginProbeDescriptorSymbolName), std::string::npos);
}

TEST(PluginSupportTest, openPluginRejectsBadProbeMagic) {
    auto opened = babelwires::openPlugin(badMagicPluginPath());

    ASSERT_FALSE(opened);
    EXPECT_NE(opened.error().toString().find("invalid plugin probe descriptor magic"), std::string::npos);
}

TEST(PluginSupportTest, openPluginRejectsFingerprintMismatch) {
    auto opened = babelwires::openPlugin(fingerprintMismatchPluginPath());

    ASSERT_FALSE(opened);
    EXPECT_NE(opened.error().toString().find("build-incompatible"), std::string::npos);
}

TEST(PluginSupportTest, openPluginRejectsMissingRegistrationSymbol) {
    auto opened = babelwires::openPlugin(missingRegistrationPluginPath());

    ASSERT_FALSE(opened);
    EXPECT_NE(opened.error().toString().find(babelwires::c_pluginRegistrationSymbolName), std::string::npos);
}