/**
 * Build fingerprint generation for loadable module compatibility checks.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

// NO DEPENDENCIES ARE PERMITTED HERE EXCEPT FOR STANDARD LIBRARY HEADERS.
//
// This code is called prior to the point at which it's known that the module is compatible
// with the main codebase.

#include <cstddef>

namespace babelwires {

    inline constexpr std::size_t c_buildFingerprintBufferSize = 1024;

    /// Write the build fingerprint of the current build into a caller-provided buffer.
    /// The build fingerprint is a string which encodes properties of the build environment and configuration
    /// which are necessary (but unfortunately not sufficient) for binary compatibility.
    ///
    /// The function has an inline implementation (in buildFingerprint_inl.hpp), so the calling code gets the
    /// fingerprint corresponding to the environment in which it was compiled (hence "my" fingerprint).
    ///
    /// The output uses a canonical key=value format with one entry per line. A schema_version comes first, but
    /// subsequent entries are sorted lexicographically.
    ///
    /// The returned value is the number of bytes used in outBuffer, including the null terminator. If output is
    /// truncated, the return value equals bufferSize. If outBuffer is null or bufferSize is zero, nothing is written
    /// and zero is returned.
    std::size_t writeMyBuildFingerprint(char* outBuffer, std::size_t bufferSize);

} // namespace babelwires

#include <BaseLib/BuildCompatibility/buildFingerprint_inl.hpp>
