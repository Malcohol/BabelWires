/**
 * The MapProject carries an enriched version of the data in a MapData and has support for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/Maps/mapData.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Serialization/serializable.hpp>

#include <vector>
#include <memory>

namespace babelwires {
    class MapProjectEntry;
    class MapEntryData;

    /// The MapProject carries an enriched version of the data in a MapData and has support for editing.
    class MapProject {
      public:
        MapProject(const ProjectContext& projectContext);
        virtual ~MapProject();

        LongIdentifier getSourceId() const;
        LongIdentifier getTargetId() const;

        void setSourceId(LongIdentifier sourceId);
        void setTargetId(LongIdentifier targetId);

        const Type* getSourceType() const;
        const Type* getTargetType() const;

        MapData extractMapData() const;
        void setMapData(const MapData& data);

        unsigned int getNumMapEntries() const;
        const MapProjectEntry& getMapEntry(unsigned int index) const;

        void addMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index);
        void removeMapEntry(unsigned int index);

        bool operator==(const MapProject& other) const;
        bool operator!=(const MapProject& other) const;

        /// Check that the entries types match the source and target ids.
        bool validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const;

      private:
        const ProjectContext& m_projectContext;
        LongIdentifier m_sourceId;
        LongIdentifier m_targetId;
        std::vector<std::unique_ptr<MapProjectEntry>> m_mapEntries;
    };
}
