/**
 * Converts MapData with a EnumWithCppEnum for a source type to another value type, using an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Maps/Helpers/enumValueAdapters.hpp>
#include <BabelWiresLib/Maps/Helpers/mapApplicatorFallbackHelper.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>

namespace babelwires {
    /// Converts MapData with a EnumWithCppEnum for a source type to another value type, using an array.
    /// This can also be used where the subtype of such an enum is expected, by using the supertype as ENUM.
    /// The extra values will not get used when the map is applied.
    template <typename ENUM, typename U> class EnumSourceMapApplicator {
      public:
        EnumSourceMapApplicator(const MapData& mapData, const ENUM& sourceEnumType, const ValueAdapter<U>& targetAdapter) {
            MapApplicatorFallbackHelper<typename ENUM::Value, U> fallbackHelper(mapData, targetAdapter);
            for (unsigned int i = 0; i < static_cast<unsigned int>(ENUM::Value::NUM_VALUES); ++i) {
                m_array[i] = fallbackHelper.getFallback(static_cast<typename ENUM::Value>(i));
            }
            const EnumToValueValueAdapter<ENUM> sourceAdapter{sourceEnumType};

            for (unsigned int i = 0; i < mapData.getNumMapEntries() - 1; ++i) {
                const MapEntryData& entryData = mapData.getMapEntry(i);
                switch (entryData.getKind()) {
                    case MapEntryData::Kind::OneToOne: {
                        const auto& maplet = static_cast<const OneToOneMapEntryData&>(entryData);
                        m_array[static_cast<unsigned int>(sourceAdapter(*maplet.getSourceValue()))] = targetAdapter(*maplet.getTargetValue());
                        break;
                    }
                    default:
                        assert(false && "Unexpected kind of map entry");
                }
            }
        }

      public:
        U operator[](const typename ENUM::Value& t) const {
            const unsigned int index = static_cast<unsigned int>(t);
            assert(index < static_cast<unsigned int>(ENUM::Value::NUM_VALUES) && "Enum queried with out-of-range value");
            return m_array[index];
        }

      private:
        U m_array[static_cast<unsigned int>(ENUM::Value::NUM_VALUES)];
    };
} // namespace babelwires
