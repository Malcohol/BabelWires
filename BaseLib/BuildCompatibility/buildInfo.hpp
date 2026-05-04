/**
 * A function that provides the build fingerprint of the host system.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <BaseLib/BuildCompatibility/buildFingerprint.hpp>

namespace babelwires {
    /// A convenience function to get the build fingerprint as a std::string. This always returns the build
    /// fingerprint of the environment that BaseLib was compiled in.
    std::string BASELIB_API getBuildFingerprint();

} // namespace babelwires
