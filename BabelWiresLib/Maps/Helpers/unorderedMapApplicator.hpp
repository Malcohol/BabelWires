/**
 * Converts MapData to a native C++ function based on a std::unordered_map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/Helpers/valueAdapter.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>

#include <unordered_map>

namespace babelwires {
    /// Converts MapData to a native C++ function based on a std::unordered_map.
    template <typename T, typename U> class UnorderedMapApplicator {
      public:
        UnorderedMapApplicator(const MapData& mapData, const ValueAdapter<T>& sourceAdapter,
                               const ValueAdapter<U>& targetAdapter)
            : m_mapData(mapData) {
            for (int i = mapData.getNumMapEntries() - 1; i >= 0; --i) {
                const MapEntryData& entryData = mapData.getMapEntry(i);
                switch (entryData.getKind()) {
                    case MapEntryData::Kind::OneToOne: {
                        const auto& maplet = static_cast<const OneToOneMapEntryData&>(entryData);
                        m_memo[sourceAdapter(*maplet.getSourceValue())] = targetAdapter(*maplet.getTargetValue());
                        break;
                    }
                    case MapEntryData::Kind::AllToOne: {
                        const auto& allToOne = static_cast<const AllToOneFallbackMapEntryData&>(entryData);
                        m_fallbackValue = targetAdapter(*allToOne.getTargetValue());
                        break;
                    }
                    case MapEntryData::Kind::AllToSame:
                        m_fallbackIsIdentity = true;
                        break;
                    default:
                        assert(false && "Unexpected kind of map entry");
                }
            }
        }

      public:
        U operator[](const T& t) const {
            const auto it = m_memo.find(t);
            if (it != m_memo.end()) {
                return it->second;
            }
            return getFallback(t);
        }

      private:
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
        const MapData& m_mapData;
        std::unordered_map<T, U> m_memo;
        U m_fallbackValue;
        bool m_fallbackIsIdentity = false;
    };
} // namespace babelwires
