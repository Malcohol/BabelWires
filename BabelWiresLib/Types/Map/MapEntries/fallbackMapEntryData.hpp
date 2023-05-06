/**
 * FallbackMapEntries are the last entry in a map, and define what happens to previously unmapped source values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

namespace babelwires {
    /// FallbackMapEntries are the last entry in a map, and define what happens to previously unmapped source values.
    class FallbackMapEntryData : public MapEntryData {
      public:
        SERIALIZABLE_ABSTRACT(FallbackMapEntryData, MapEntryData);
    };
} // namespace babelwires
