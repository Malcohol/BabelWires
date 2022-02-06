/**
 * A MapFeature allows the user to define a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/heavyValueFeature.hpp>
#include <BabelWiresLib/Maps/map.hpp>

#include <unordered_set>

namespace babelwires {
    class MapFeature : public HeavyValueFeature<Map> {
      public:
        using TypeSet = std::unordered_set<LongIdentifier>;

        MapFeature(TypeSet allowedSourceIds, TypeSet allowedTargetIds);

      protected:
        std::string doGetValueType() const override;
        void onBeforeSetValue(const Map& newValue) const override;

      public:
        const TypeSet& getAllowedSourceIds() const;
        const TypeSet& getAllowedTargetIds() const;

      private:
        /// Empty means all types are allowed.
        TypeSet m_allowedSourceIds;
        /// Empty means all types are allowed.
        TypeSet m_allowedTargetIds;
    };
} // namespace babelwires
