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
#include <Common/Utilities/result.hpp>

#include <vector>
#include <memory>

namespace babelwires {
    class MapProjectEntry;
    class MapEntryData;
    struct ProjectContext;

    /// The MapProject carries an enriched version of the data in a MapData and has support for editing.
    class MapProject {
      public:
        MapProject(const ProjectContext& projectContext);
        virtual ~MapProject();

        void setAllowedSourceTypeId(LongIdentifier sourceTypeId);
        void setAllowedTargetTypeId(LongIdentifier targetTypeId);

        LongIdentifier getAllowedSourceTypeId() const;
        LongIdentifier getAllowedTargetTypeId() const;
       
        LongIdentifier getSourceTypeId() const;
        LongIdentifier getTargetTypeId() const;

        void setSourceTypeId(LongIdentifier sourceId);
        void setTargetTypeId(LongIdentifier targetId);

        const Type* getSourceType() const;
        const Type* getTargetType() const;

        MapData extractMapData() const;
        void setMapData(const MapData& data);

        unsigned int getNumMapEntries() const;
        const MapProjectEntry& getMapEntry(unsigned int index) const;

        void addMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index);
        void removeMapEntry(unsigned int index);
        void replaceMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index);

        bool operator==(const MapProject& other) const;
        bool operator!=(const MapProject& other) const;

        /// Check that the entries types match the source and target ids.
        Result validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const;

        const ProjectContext& getProjectContext() const;

      private:
        const ProjectContext& m_projectContext;
        /// Empty means all types are allowed.
        LongIdentifier m_allowedSourceTypeId;
        /// Empty means all types are allowed.
        LongIdentifier m_allowedTargetTypeId;
        LongIdentifier m_sourceTypeId;
        LongIdentifier m_targetTypeId;
        std::vector<std::unique_ptr<MapProjectEntry>> m_mapEntries;
    };
}
