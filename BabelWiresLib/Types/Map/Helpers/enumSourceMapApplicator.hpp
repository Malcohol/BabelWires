/**
 * Converts MapValue with a EnumWithCppEnum for a source type to another value type, using an array.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Enum/enumType.hpp>
#include <BabelWiresLib/Types/Map/Helpers/enumValueAdapters.hpp>
#include <BabelWiresLib/Types/Map/Helpers/mapApplicatorFallbackHelper.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

namespace babelwires {

    /// Converts MapValue with a EnumType source type, to a map between source indices and given type U.
    /// This can also be used where the subtype of such an enum is expected, by using the supertype as ENUM.
    /// NOTE: In the cases where the MapValue allows AllToSame, U must be unsigned int.
    template <typename U> class EnumSourceIndexMapApplicator {
      public:
        EnumSourceIndexMapApplicator(const MapValue& mapValue, const EnumType& sourceEnumType,
                                     const ValueAdapter<U>& targetAdapter) : m_srcEnum(sourceEnumType) {
            const unsigned int numEnumValues = sourceEnumType.getValueSet().size();
            m_indexToTarget.resize(numEnumValues);
            MapApplicatorFallbackHelper<unsigned int, U> fallbackHelper(mapValue, targetAdapter);
            for (unsigned int i = 0; i < numEnumValues; ++i) {
                m_indexToTarget[i] = fallbackHelper.getFallback(i);
            }
            const EnumToIndexValueAdapter sourceAdapter{sourceEnumType};

            for (unsigned int i = 0; i < mapValue.getNumMapEntries() - 1; ++i) {
                const MapEntryData& entryData = mapValue.getMapEntry(i);
                    m_indexToTarget[sourceAdapter(*entryData.getSourceValue())] =
                        targetAdapter(*entryData.getTargetValue());
            }
        }

        U operator[](unsigned int srcIndex) const {
            assert((srcIndex < m_srcEnum.getValueSet().size()) && "EnumType queried with out-of-range value");
            return m_indexToTarget[srcIndex];
        }

      private:
        const EnumType& m_srcEnum;
        std::vector<U> m_indexToTarget;
    };

    /// Converts MapValue with a EnumWithCppEnum for a source type to another value type, using an array.
    /// This can also be used where the subtype of such an enum is expected, by using the supertype as ENUM.
    /// The extra values will not get used when the map is applied.
    /// NOTE: In the cases where the MapValue allows AllToSame, U must be the same as ENUM.
    template <typename ENUM, typename U> class EnumSourceMapApplicator {
      public:
        EnumSourceMapApplicator(const MapValue& mapValue, const ENUM& sourceEnumType, const ValueAdapter<U>& targetAdapter) {
            MapApplicatorFallbackHelper<typename ENUM::Value, U> fallbackHelper(mapValue, targetAdapter);
            for (unsigned int i = 0; i < static_cast<unsigned int>(ENUM::Value::NUM_VALUES); ++i) {
                m_array[i] = fallbackHelper.getFallback(static_cast<typename ENUM::Value>(i));
            }
            const EnumToValueValueAdapter<ENUM> sourceAdapter{sourceEnumType};

            for (unsigned int i = 0; i < mapValue.getNumMapEntries() - 1; ++i) {
                const MapEntryData& entryData = mapValue.getMapEntry(i);
                m_array[static_cast<unsigned int>(sourceAdapter(*entryData.getSourceValue()))] = targetAdapter(*entryData.getTargetValue());
            }
        }

        U operator[](const typename ENUM::Value& t) const {
            const unsigned int index = static_cast<unsigned int>(t);
            assert(index < static_cast<unsigned int>(ENUM::Value::NUM_VALUES) && "EnumType queried with out-of-range value");
            return m_array[index];
        }

      private:
        U m_array[static_cast<unsigned int>(ENUM::Value::NUM_VALUES)];
    };
} // namespace babelwires
