/**
 * A helper which handles fallbacks for MapApplicators.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Enum/enum.hpp>
#include <BabelWiresLib/Types/Map/Helpers/valueAdapter.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapData.hpp>

namespace babelwires {
    /// A helper which handles fallbacks for MapApplicators.
    /// NOTE: In the cases where the MapData allows an AllToSame fallback, T and U must be the same.
    /// (It would be possible to provide MapApplicators without this limitation, but it's pretty awkward.)
    template <typename T, typename U> class MapApplicatorFallbackHelper {
      public:
        MapApplicatorFallbackHelper(const MapData& mapData, const ValueAdapter<U>& targetAdapter) {
            const MapEntryData& fallbackEntryData = mapData.getMapEntry(mapData.getNumMapEntries() - 1);
            switch (fallbackEntryData.getKind()) {
                case MapEntryData::Kind::All21: {
                    const auto& allToOne = static_cast<const AllToOneFallbackMapEntryData&>(fallbackEntryData);
                    m_fallbackValue = targetAdapter(*allToOne.getTargetValue());
                    break;
                }
                case MapEntryData::Kind::All2Sm:
#ifndef NDEBUG
                {
                    constexpr bool sourceAndTargetAreSame = std::is_same_v<T, U>;
                    assert(sourceAndTargetAreSame && "You cannot use a mapApplicator with an AllToSame fallback if the "
                                                     "native source and target types do not agree");
                }
#endif
                    m_fallbackIsIdentity = true;
                    break;
                default:
                    assert(false && "The fallback was an unexpected kind of map entry");
            }
        }

        template <typename V = T> std::enable_if_t<std::is_same_v<V, U>, U> getFallback(const T& t) const {
            if (m_fallbackIsIdentity) {
                return t;
            } else {
                return m_fallbackValue;
            }
        }

        template <typename V = T> std::enable_if_t<!std::is_same_v<V, U>, U> getFallback(const T& t) const {
            return m_fallbackValue;
        }

      private:
        U m_fallbackValue;
        bool m_fallbackIsIdentity = false;
    };
} // namespace babelwires