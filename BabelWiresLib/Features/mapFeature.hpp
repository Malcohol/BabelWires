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

namespace babelwires {
    class MapFeature : public HeavyValueFeature<Map> {
      public:
        using TypeSet = std::vector<LongIdentifier>;

        /// If the sets are empty, any types are allowed.
        /// If the sets are not empty, then the first type is taken to be the default.
        MapFeature(TypeSet allowedSourceIds, TypeSet allowedTargetIds);

      protected:
        std::string doGetValueType() const override;
        void onBeforeSetValue(const Map& newValue) const override;
        void doSetToDefault() override;

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
