/**
 * IdentityFallbackMapEntryData map everything in the source to itself.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/MapEntries/fallbackMapEntryData.hpp>

namespace babelwires {
    /// FallbackMapEntries are the last entry in a map, and define what happens to previously unmapped source values.
    class IdentityFallbackMapEntryData : public FallbackMapEntryData {
      public:
        std::size_t getHash() const override {
            // "Id Fallback" (an arbitrary hash value)
            return 0x1DFA77BC;
        }
        bool operator==(const MapEntryData& other) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        bool isValid(const Type& sourceType, const Type& targetType) const override;
    };
}
