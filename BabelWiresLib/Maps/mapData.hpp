/**
 * MapData stores the mappings which define a map.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>
#include "Common/Serialization/serializable.hpp"
#include "BabelWiresLib/Project/projectVisitable.hpp"

#include <vector>
#include <memory>

namespace babelwires {
    class MapEntryData;
    class ProjectContext;

    /// The data held by a map.
    /// To support scenarios such as deserialization after an awkward versioning step, or copy and
    /// paste from similar but non-identical types, entries are not presumed to be valid.
    /// However, an assignment of MapData to a MapFeature will fail if there are
    /// invalid entries.
    class MapData : public Serializable, public ProjectVisitable {
      public:
        SERIALIZABLE(MapData, "mapData", void, 1);

        MapData();
        MapData(const MapData& other);
        MapData(MapData&& other);
        MapData& operator=(const MapData& other);
        MapData& operator=(MapData&& other);
        virtual ~MapData();

        LongIdentifier getSourceId() const;
        LongIdentifier getTargetId() const;

        void setSourceId(LongIdentifier sourceId);
        void setTargetId(LongIdentifier targetId);

        unsigned int getNumMapEntries() const;
        const MapEntryData& getMapEntry(unsigned int index) const;

        void emplaceBack(std::unique_ptr<MapEntryData> newEntry);

        bool operator==(const MapData& other) const;
        bool operator!=(const MapData& other) const;

        std::size_t getHash() const;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// Check that the entries types match the source and target ids.
        static std::string validateEntryData(const ProjectContext& context, LongIdentifier sourceId, LongIdentifier targetId, const MapEntryData& entryData);
        bool hasInvalidEntries(const ProjectContext& context) const;

      public:
        LongIdentifier m_sourceId;
        LongIdentifier m_targetId;
        std::vector<std::unique_ptr<MapEntryData>> m_mapEntries;
    };
}
