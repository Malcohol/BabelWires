/**
 * A MapFeature describes a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

namespace babelwires {
    /// A MapFeature describes a mapping between types.
    /// All the entries in the MapValue of a MapFeature are expected to be valid.
    class MapFeature2 : public SimpleValueFeature {
      public:
        MapFeature2(const TypeRef& sourceType, const TypeRef& targetType,
                    MapEntryData::Kind defaultFallbackKind = MapEntryData::Kind::All21);

        const MapValue& get() const;
    };
} // namespace babelwires
