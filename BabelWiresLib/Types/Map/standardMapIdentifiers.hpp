/**
 * Some standard identifiers commonly used by maps or the map UI.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    constexpr const char c_wildcard[] = "*";
    constexpr const char c_wildcardMatch[] = "<match>";

    babelwires::ShortId getWildcardId();
    babelwires::ShortId getWildcardMatchId();
}
