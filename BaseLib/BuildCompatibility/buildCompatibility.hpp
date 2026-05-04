/**
 * Build fingerprint compatibility checks.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>
#include <BaseLib/Result/result.hpp>

#include <string_view>

namespace babelwires {

    /// Compare two build fingerprints represented as textual buffers.
    /// A single trailing newline is ignored, and either fingerprint may extend the other so long as the shorter one
    /// is an exact prefix of the longer one.
    BASELIB_API Result compareBuildFingerprints(std::string_view expected, std::string_view actual);

} // namespace babelwires
