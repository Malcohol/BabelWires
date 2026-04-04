/**
 * Some standard identifiers commonly used by maps or the map UI.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BaseLib/Identifiers/identifier.hpp>

namespace babelwires {
    constexpr const char c_wildcard[] = "*";
    constexpr const char c_wildcardMatch[] = "<match>";
    constexpr const char c_blankName[] = "____";

    BABELWIRESLIB_API ShortId getWildcardId();
    BABELWIRESLIB_API ShortId getWildcardMatchId();
    BABELWIRESLIB_API ShortId getBlankValueId();
}
