#include <gtest/gtest.h>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/PluginSupport/pluginManager.hpp>
#include <BaseLib/PluginSupport/pluginOperations.hpp>
#include <BaseLib/Version/version.hpp>

#include <Tests/TestUtils/testLog.hpp>

#include <string>

namespace {
    std::filesystem::path pluginOutputDir() {
        return std::filesystem::path(BASELIB_TEST_PLUGIN_OUTPUT_DIR);
    }

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

TEST(PluginSupportTest, discoverPluginsFindsPluginsWithCorrectExtension) {
    testUtils::TestLog log;
    auto discovered =
        babelwires::discoverPlugins(pluginOutputDir(), ".bwplugin", log);

    ASSERT_TRUE(discovered) << discovered.error().toString();
    EXPECT_NE(std::find(discovered->begin(), discovered->end(), goodPluginPath()), discovered->end());
    EXPECT_NE(std::find(discovered->begin(), discovered->end(), badMagicPluginPath()), discovered->end());
    EXPECT_NE(std::find(discovered->begin(), discovered->end(), fingerprintMismatchPluginPath()), discovered->end());
    EXPECT_NE(std::find(discovered->begin(), discovered->end(), missingProbePluginPath()), discovered->end());
    EXPECT_NE(std::find(discovered->begin(), discovered->end(), missingRegistrationPluginPath()), discovered->end());
}

TEST(PluginSupportTest, discoverPluginsSkipsFilesWithWrongExtension) {
    testUtils::TestLog log;
    auto discovered =
        babelwires::discoverPlugins(pluginOutputDir(), ".notaplugin", log);

    ASSERT_TRUE(discovered) << discovered.error().toString();
    EXPECT_EQ(discovered->size(), 0);
}

TEST(PluginSupportTest, pluginManagerLoadGoodPlugin) {
    testUtils::TestLogWithListener log;
    babelwires::PluginManager manager(".bwplugin");

    auto good = babelwires::openPlugin(goodPluginPath());
    ASSERT_TRUE(good) << good.error().toString();

    babelwires::Context context;
    auto loadGood = manager.loadPlugin(std::move(*good), context, log);
    ASSERT_TRUE(loadGood) << loadGood.error().toString();
    EXPECT_TRUE(log.hasSubstring("Successfully loaded plugin \"" + goodPluginPath().string()));
}

TEST(PluginSupportTest, pluginManagerLoadPluginRejectsDuplicateUuid) {
    testUtils::TestLog log;
    babelwires::PluginManager manager(".bwplugin");

    auto first = babelwires::openPlugin(goodPluginPath());
    ASSERT_TRUE(first) << first.error().toString();
    auto second = babelwires::openPlugin(goodPluginPath());
    ASSERT_TRUE(second) << second.error().toString();

    babelwires::Context context;
    auto loadFirst = manager.loadPlugin(std::move(*first), context, log);
    ASSERT_TRUE(loadFirst) << loadFirst.error().toString();
    auto loadSecond = manager.loadPlugin(std::move(*second), context, log);
    ASSERT_FALSE(loadSecond);
    EXPECT_NE(loadSecond.error().toString().find("duplicates an already loaded plugin UUID"), std::string::npos);
}

TEST(PluginSupportTest, pluginManagerLoadAllPlugins) {
    testUtils::TestLogWithListener log;
    babelwires::PluginManager manager(".bwplugin");

    babelwires::Context context;
    const int numLoaded = manager.loadAllPlugins(pluginOutputDir(), context, log);
    EXPECT_EQ(numLoaded, 1);
    EXPECT_TRUE(log.hasSubstring("Successfully loaded plugin \"" + goodPluginPath().string()));
    EXPECT_FALSE(log.hasSubstring("Successfully loaded plugin \"" + badMagicPluginPath().string()));
    EXPECT_TRUE(log.hasSubstring(badMagicPluginPath().string()));
}

