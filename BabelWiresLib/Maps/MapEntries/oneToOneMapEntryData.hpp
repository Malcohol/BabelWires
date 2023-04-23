/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <Common/Hash/hash.hpp>

namespace babelwires {
    class TypeSystem;

    /// A map entry with a single source value of discrete type.
    class OneToOneMapEntryData : public MapEntryData {
      public:
        CLONEABLE(OneToOneMapEntryData);
        SERIALIZABLE(OneToOneMapEntryData, "oneToOne", MapEntryData, 1);
        OneToOneMapEntryData(const TypeSystem& typeSystem, const TypeRef& sourceTypeRef, const TypeRef& targetTypeRef);
        OneToOneMapEntryData(const OneToOneMapEntryData& other);
        OneToOneMapEntryData(OneToOneMapEntryData&& other);

        std::size_t getHash() const override;
        bool operator==(const MapEntryData& other) const override;

        const EditableValueHolder& getSourceValue() const;
        void setSourceValue(EditableValueHolder value);
        const EditableValueHolder& getTargetValue() const;
        void setTargetValue(EditableValueHolder value);

        Kind getKind() const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;
      
      protected:
        Result doValidate(const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType) const override;

        /// Needed for deserialization.
        OneToOneMapEntryData();

      private:
        /// Non-null
        EditableValueHolder m_sourceValue;
        /// Non-null
        EditableValueHolder m_targetValue;
    };
} // namespace babelwires
