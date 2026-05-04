/**
 * Build fingerprint compatibility checks.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/BuildCompatibility/buildCompatibility.hpp>

#include <BaseLib/Result/resultDSL.hpp>

namespace {
    void trimTrailingNewline(std::string_view& fingerprint) {
        if (!fingerprint.empty() && fingerprint.back() == '\n') {
            fingerprint.remove_suffix(1);
        }
    }
} // namespace

babelwires::Result babelwires::compareBuildFingerprints(std::string_view expected, std::string_view actual) {
    const std::string_view originalExpected = expected;
    const std::string_view originalActual = actual;

    trimTrailingNewline(expected);
    trimTrailingNewline(actual);

    const std::string_view shorter = (expected.size() <= actual.size()) ? expected : actual;
    const std::string_view longer = (expected.size() <= actual.size()) ? actual : expected;

    if (longer.substr(0, shorter.size()) == shorter) {
        return Result{};
    }

    return Error() << "Build fingerprints do not match.\nExpected:\n" << originalExpected << "\nActual:\n"
                   << originalActual;
}
