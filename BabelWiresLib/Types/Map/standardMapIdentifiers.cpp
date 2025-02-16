/**
 * Some standard identifiers commonly used by maps or the map UI.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/standardMapIdentifiers.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

babelwires::ShortId babelwires::getWildcardId() {
    return BW_SHORT_ID("_wild", c_wildcard, "6e51fc3f-de0b-4111-92bb-7d843223ae25");
}

babelwires::ShortId babelwires::getWildcardMatchId()
{
    return BW_SHORT_ID("_match", c_wildcardMatch, "eae0f1ad-630b-4c64-911e-c38572a5e5ae");
}
