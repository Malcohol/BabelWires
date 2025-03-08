/**
 * FallbackMapEntries are the last entry in a map, and define what happens to previously unmapped source values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Types/Map/MapEntries/fallbackMapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {

    class AllToOneFallbackMapEntryData : public FallbackMapEntryData {
      public:
        CLONEABLE(AllToOneFallbackMapEntryData);
        SERIALIZABLE(AllToOneFallbackMapEntryData, "allToOne", FallbackMapEntryData, 1);

        AllToOneFallbackMapEntryData(const TypeSystem& typeSystem, const TypeRef& targetTypeRef);
        AllToOneFallbackMapEntryData(const AllToOneFallbackMapEntryData& other);
        AllToOneFallbackMapEntryData(AllToOneFallbackMapEntryData&& other);

        const EditableValueHolder* tryGetTargetValue() const override;
        void setTargetValue(EditableValueHolder value);

        std::size_t getHash() const override;
        bool operator==(const MapEntryData& other) const override;

        Kind getKind() const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

      protected:
        Result doValidate(const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType) const override;

        /// Needed for deserialization.
        AllToOneFallbackMapEntryData();
        
      private:
        /// Non-null
        EditableValueHolder m_targetValue;
    };
} // namespace babelwires
