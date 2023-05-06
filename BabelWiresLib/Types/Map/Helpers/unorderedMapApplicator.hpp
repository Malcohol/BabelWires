/**
 * Converts MapValue to a native C++ function based on a std::unordered_map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/Helpers/valueAdapter.hpp>
#include <BabelWiresLib/Types/Map/Helpers/mapApplicatorFallbackHelper.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

#include <unordered_map>

namespace babelwires {
    /// Converts MapValue to a native C++ function based on a std::unordered_map.
    /// NOTE: In the cases where the MapValue allows an AllToSame fallback, T and U must be the same.
    template <typename T, typename U> class UnorderedMapApplicator {
      public:
        UnorderedMapApplicator(const MapValue& mapValue, const ValueAdapter<T>& sourceAdapter,
                               const ValueAdapter<U>& targetAdapter) : m_fallbackHelper(mapValue, targetAdapter) {
            for (unsigned int i = 0; i < mapValue.getNumMapEntries() - 1; ++i) {
                const MapEntryData& entryData = mapValue.getMapEntry(i);
                switch (entryData.getKind()) {
                    case MapEntryData::Kind::One21: {
                        const auto& maplet = static_cast<const OneToOneMapEntryData&>(entryData);
                        m_map[sourceAdapter(*maplet.getSourceValue())] = targetAdapter(*maplet.getTargetValue());
                        break;
                    }
                    default:
                        assert(false && "Unexpected kind of map entry");
                }
            }
        }

      public:
        U operator[](const T& t) const {
            const auto it = m_map.find(t);
            if (it != m_map.end()) {
                return it->second;
            }
            return m_fallbackHelper.getFallback(t);
        }

      private:
        MapApplicatorFallbackHelper<T, U> m_fallbackHelper;
        std::unordered_map<T, U> m_map;
    };
} // namespace babelwires
