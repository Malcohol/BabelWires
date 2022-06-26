/**
 * MapData stores the mappings which define a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Serialization/serializable.hpp>
#include <Common/Utilities/result.hpp>
#include <Common/Cloning/cloneable.hpp>

#include <vector>
#include <memory>

namespace babelwires {
    class MapEntryData;
    class TypeSystem;
    class Type;

    /// The data held by a map.
    /// To support scenarios such as deserialization after an awkward versioning step, or copy and
    /// paste from similar but non-identical types, entries are not presumed to be valid.
    /// However, an assignment of MapData to a MapFeature will fail if there are
    /// invalid entries.
    class MapData : public Serializable, public Cloneable, public ProjectVisitable {
      public:
        SERIALIZABLE(MapData, "mapData", void, 1);
        CLONEABLE(MapData);

        MapData();
        MapData(const MapData& other);
        MapData(MapData&& other);
        MapData& operator=(const MapData& other);
        MapData& operator=(MapData&& other);
        virtual ~MapData();

        LongIdentifier getSourceTypeId() const;
        LongIdentifier getTargetTypeId() const;

        void setSourceTypeId(LongIdentifier sourceId);
        void setTargetTypeId(LongIdentifier targetId);

        unsigned int getNumMapEntries() const;
        const MapEntryData& getMapEntry(unsigned int index) const;

        /// The entries have a single fallback which maps everything to the default target value.
        void setEntriesToDefault(const TypeSystem& typeSystem);

        void emplaceBack(std::unique_ptr<MapEntryData> newEntry);

        bool operator==(const MapData& other) const;
        bool operator!=(const MapData& other) const;

        std::size_t getHash() const;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        bool isValid(const TypeSystem& typeSystem) const;

      public:
        LongIdentifier m_sourceTypeId;
        LongIdentifier m_targetTypeId;
        /// All non-null.
        std::vector<std::unique_ptr<MapEntryData>> m_mapEntries;
    };
}
