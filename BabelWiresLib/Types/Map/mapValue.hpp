/**
 * MapValue stores the mappings which define a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/editableValue.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Utilities/result.hpp>

#include <vector>
#include <memory>

namespace babelwires {
    class TypeSystem;
    class Type;

    /// The data held by a map.
    /// To support scenarios such as deserialization after an awkward versioning step, or copy and
    /// paste from similar but non-identical types, entries are not presumed to be valid.
    /// However, an assignment of MapValue will fail if there are invalid entries.
    class MapValue : public AlwaysEditableValue {
      public:
        SERIALIZABLE(MapValue, "mapValue", EditableValue, 1);
        CLONEABLE(MapValue);

        MapValue();
        MapValue(const MapValue& other);
        MapValue(MapValue&& other);
        MapValue(const TypeSystem& typeSystem, TypeExp sourceRef, TypeExp targetRef, MapEntryData::Kind fallbackKind);

        MapValue& operator=(const MapValue& other);
        MapValue& operator=(MapValue&& other);
        virtual ~MapValue();

        const TypeExp& getSourceTypeRef() const;
        const TypeExp& getTargetTypeRef() const;

        void setSourceTypeRef(const TypeExp& sourceRef);
        void setTargetTypeRef(const TypeExp& targetRef);

        unsigned int getNumMapEntries() const;
        const MapEntryData& getMapEntry(unsigned int index) const;

        /// The entries have a single fallback which maps everything to the default target value.
        void setEntriesToDefault(const TypeSystem& typeSystem);

        void emplaceBack(std::unique_ptr<MapEntryData> newEntry);

        bool operator==(const MapValue& other) const;

        bool operator==(const Value& other) const override;
        std::size_t getHash() const override;
        std::string toString() const override;
        bool canContainIdentifiers() const override;
        bool canContainFilePaths() const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        bool isValid(const TypeSystem& typeSystem) const;

      public:
        TypeExp m_sourceTypeRef;
        TypeExp m_targetTypeRef;
        /// All non-null.
        std::vector<std::unique_ptr<MapEntryData>> m_mapEntries;
    };
}
