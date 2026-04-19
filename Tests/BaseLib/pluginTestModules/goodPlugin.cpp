#include <BaseLib/PluginSupport/pluginDescriptor.hpp>

#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>
#include <BaseLib/Version/codebaseVersion_gen.hpp>

namespace {
    babelwires::Result testGoodPluginRegister(babelwires::Context&, babelwires::UserAdvisoryLogger&) {
        return babelwires::Result{};
    }
}

#define PLUGIN_REGISTRATION_FUNCTION testGoodPluginRegister
#define PLUGIN_UUID "11111111-2222-3333-4444-555555555555"
#include <BaseLib/PluginSupport/Detail/pluginEntryBoilerplate.inc>