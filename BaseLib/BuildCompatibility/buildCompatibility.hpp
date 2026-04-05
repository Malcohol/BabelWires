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
    /// Exact byte-equality is required.
    BASELIB_API Result compareBuildFingerprints(std::string_view expected, std::string_view actual);

} // namespace babelwires
