/**
 * Converts MapData to a native C++ function based on a std::unordered_map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/Helpers/valueAdapter.hpp>
#include <BabelWiresLib/Maps/Helpers/mapApplicatorBase.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>

#include <unordered_map>

namespace babelwires {
    /// Converts MapData to a native C++ function based on a std::unordered_map.
    template <typename T, typename U> class UnorderedMapApplicator : MapApplicatorBase<T, U> {
      public:
        UnorderedMapApplicator(const MapData& mapData, const ValueAdapter<T>& sourceAdapter,
                               const ValueAdapter<U>& targetAdapter) : MapApplicatorBase<T,U>(mapData, targetAdapter) {
            for (unsigned int i = 0; i < mapData.getNumMapEntries() - 1; ++i) {
                const MapEntryData& entryData = mapData.getMapEntry(i);
                switch (entryData.getKind()) {
                    case MapEntryData::Kind::OneToOne: {
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
            return this->getFallback(t);
        }

      private:
        std::unordered_map<T, U> m_map;
    };
} // namespace babelwires
