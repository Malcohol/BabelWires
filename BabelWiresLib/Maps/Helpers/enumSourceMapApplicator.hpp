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
#include <BabelWiresLib/Maps/Helpers/mapApplicatorBase.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>

namespace babelwires {
    /// Converts MapData with a EnumWithCppEnum for a source type to another value type, using an array.
    template <typename ENUM, typename U> class EnumSourceMapApplicator : MapApplicatorBase<typename ENUM::Value, U> {
      public:
        EnumSourceMapApplicator(const MapData& mapData, const ENUM& sourceEnumType, const ValueAdapter<U>& targetAdapter) : MapApplicatorBase<typename ENUM::Value, U>(mapData, targetAdapter) {
            for (unsigned int i = 0; i < static_cast<unsigned int>(ENUM::Value::NUM_VALUES); ++i) {
                m_array[i] = this->getFallback(static_cast<typename ENUM::Value>(i));
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
            return m_array[static_cast<unsigned int>(t)];
        }

      private:
        U m_array[static_cast<unsigned int>(ENUM::Value::NUM_VALUES)];
    };
} // namespace babelwires
