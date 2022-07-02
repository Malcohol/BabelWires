/**
 * A MapFeature describes a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/heavyValueFeature.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>

namespace babelwires {
    /// A MapFeature describes a mapping between types.
    /// All the entries in the MapData of a MapFeature are expected to be valid.
    ///
    /// Type theory note: Maps have a fallback behaviour and therefore their source
    /// types do not have to treated contravariantly. Instead, map values can be used if
    /// they have a "related" source type (i.e. in a subtype or supertype relationship).
    /// Maps with unrelated source types wouldn't be useful, so I judged it better to
    /// exclude them.
    /// Target types are treated in the usual covariant way.
    class MapFeature : public HeavyValueFeature<MapData> {
      public:
        MapFeature(LongIdentifier sourceId, LongIdentifier targetId);

      protected:
        std::string doGetValueType() const override;
        void onBeforeSetValue(const MapData& newValue) const override;
        void doSetToDefault() override;

      public:
        LongIdentifier getSourceTypeId() const;
        LongIdentifier getTargetTypeId() const;

      private:
        /// Empty means all types are allowed.
        LongIdentifier m_sourceTypeId;
        /// Empty means all types are allowed.
        LongIdentifier m_targetTypeId;
    };
} // namespace babelwires
