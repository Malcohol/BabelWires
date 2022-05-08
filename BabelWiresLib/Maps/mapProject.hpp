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
#include <BabelWiresLib/TypeSystem/typeIdSet.hpp>

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

        void setAllowedSourceTypeIds(const TypeIdSet& typeSet);
        void setAllowedTargetTypeIds(const TypeIdSet& typeSet);

        const TypeIdSet& getAllowedSourceTypeIds() const;
        const TypeIdSet& getAllowedTargetTypeIds() const;
       
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
        bool validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const;

        const ProjectContext& getProjectContext() const;

        /// Convenience method which returns the first allowed source type id or int.
        LongIdentifier getDefaultSourceId() const;

        /// Convenience method which returns the first allowed target type id or int.
        LongIdentifier getDefaultTargetId() const;

      private:
        const ProjectContext& m_projectContext;
        /// Empty means all types are allowed.
        TypeIdSet m_allowedSourceTypeIds;
        /// Empty means all types are allowed.
        TypeIdSet m_allowedTargetTypeIds;
        LongIdentifier m_sourceTypeId;
        LongIdentifier m_targetTypeId;
        std::vector<std::unique_ptr<MapProjectEntry>> m_mapEntries;
    };
}
