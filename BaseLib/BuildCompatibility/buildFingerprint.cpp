/**
 * Build fingerprint generation for loadable module compatibility checks.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>

#include <cassert>

std::string babelwires::getBuildFingerprint() {
    char buffer[c_buildFingerprintBufferSize];
    std::size_t bytesUsed = writeMyBuildFingerprint(buffer, c_buildFingerprintBufferSize);
    if (bytesUsed == 0) {
        return {};
    }
    assert(bytesUsed <= c_buildFingerprintBufferSize);
    return std::string(buffer, bytesUsed - 1); // Exclude null terminator
}
