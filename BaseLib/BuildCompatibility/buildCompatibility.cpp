/**
 * Build fingerprint compatibility checks.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/BuildCompatibility/buildCompatibility.hpp>

#include <BaseLib/Result/resultDSL.hpp>

babelwires::Result babelwires::compareBuildFingerprints(std::string_view expected, std::string_view actual) {
    if (expected != actual) {
        return Error() << "Build fingerprints do not match.\nExpected:\n" << expected << "\nActual:\n" << actual;
    }
    return Result{};
}
