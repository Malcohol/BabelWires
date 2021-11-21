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
      protected:
        std::string doGetValueType() const override;
        void onBeforeSetValue(const Map& newValue) const override;

      private:
        /// Empty means all types are allowed.
        std::unordered_set<LongIdentifier> m_allowedSourceIds;
        /// Empty means all types are allowed.
        std::unordered_set<LongIdentifier> m_allowedTargetIds;
    };
} // namespace babelwires
