/**
 * FallbackMapEntries are the last entry in a map, and define what happens to previously unmapped source values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/MapEntries/fallbackMapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

namespace babelwires {

    class AllToOneFallbackMapEntryData : public FallbackMapEntryData {
      public:
        CLONEABLE(AllToOneFallbackMapEntryData);
        SERIALIZABLE(AllToOneFallbackMapEntryData, "allToOne", FallbackMapEntryData, 1);

        AllToOneFallbackMapEntryData();
        AllToOneFallbackMapEntryData(const TypeSystem& typeSystem, LongIdentifier targetTypeId);
        AllToOneFallbackMapEntryData(const AllToOneFallbackMapEntryData& other);
        AllToOneFallbackMapEntryData(AllToOneFallbackMapEntryData&& other);

        const Value* getTargetValue() const;
        void setTargetValue(std::unique_ptr<Value> value);

        std::size_t getHash() const override;
        bool operator==(const MapEntryData& other) const override;

        Kind getKind() const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

      protected:
        Result doValidate(const Type& sourceType, const Type& targetType) const override;

      private:
        std::unique_ptr<Value> m_targetValue;
    };
} // namespace babelwires
