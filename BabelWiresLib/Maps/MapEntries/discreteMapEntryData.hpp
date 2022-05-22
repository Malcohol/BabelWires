/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Utilities/hash.hpp>

namespace babelwires {
    class TypeSystem;

    /// A map entry with a single source value of discrete type.
    class DiscreteMapEntryData : public MapEntryData {
      public:
        CLONEABLE(DiscreteMapEntryData);
        SERIALIZABLE(DiscreteMapEntryData, "oneToOne", MapEntryData, 1);
        DiscreteMapEntryData();
        DiscreteMapEntryData(const TypeSystem& typeSystem, LongIdentifier sourceTypeId, LongIdentifier targetTypeId);
        DiscreteMapEntryData(const DiscreteMapEntryData& other);
        DiscreteMapEntryData(DiscreteMapEntryData&& other);

        std::size_t getHash() const override;
        bool operator==(const MapEntryData& other) const override;

        const Value* getSourceValue() const;
        void setSourceValue(std::unique_ptr<Value> value);
        const Value* getTargetValue() const;
        void setTargetValue(std::unique_ptr<Value> value);

        Kind getKind() const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
      
      protected:
        Result doValidate(const Type& sourceType, const Type& targetType) const override;

      private:
        std::unique_ptr<Value> m_sourceValue;
        std::unique_ptr<Value> m_targetValue;
    };
} // namespace babelwires
