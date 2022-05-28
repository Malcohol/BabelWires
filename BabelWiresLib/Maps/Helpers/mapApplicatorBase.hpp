/**
 * A helper which handles fallbacks for MapApplicators.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Maps/Helpers/valueAdapter.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>

namespace babelwires {
    /// A helper which handles fallbacks for MapApplicators.
    template <typename T, typename U> class MapApplicatorBase {
      public:
        MapApplicatorBase(const MapData& mapData, const ValueAdapter<U>& targetAdapter) {
            const MapEntryData& fallbackEntryData = mapData.getMapEntry(mapData.getNumMapEntries() - 1);
            switch (fallbackEntryData.getKind()) {
                case MapEntryData::Kind::AllToOne: {
                    const auto& allToOne = static_cast<const AllToOneFallbackMapEntryData&>(fallbackEntryData);
                    m_fallbackValue = targetAdapter(*allToOne.getTargetValue());
                    break;
                }
                case MapEntryData::Kind::AllToSame:
                    // TODO Assert if types are wrong.
                    m_fallbackIsIdentity = true;
                    break;
                default:
                    assert(false && "The fallback was an unexpected kind of map entry");
            }
        }

      protected:
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