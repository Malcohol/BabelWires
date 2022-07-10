/**
 * AllToSameFallbackMapEntryData map everything in the source to itself.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/MapEntries/fallbackMapEntryData.hpp>

namespace babelwires {
    /// FallbackMapEntries are the last entry in a map, and define what happens to previously unmapped source values.
    class AllToSameFallbackMapEntryData : public FallbackMapEntryData {
      public:
        CLONEABLE(AllToSameFallbackMapEntryData);
        SERIALIZABLE(AllToSameFallbackMapEntryData, "allToSame", FallbackMapEntryData, 1);

        std::size_t getHash() const override;
        bool operator==(const MapEntryData& other) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
        Kind getKind() const override;
      
      protected:
        Result doValidate(const Type& sourceType, const Type& targetType) const override;
    };
}
