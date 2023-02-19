/**
 * The MapProject carries an enriched version of the data in a MapData and has support for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/Features/mapFeature.hpp>
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

        void setAllowedSourceTypeId(const MapFeature::AllowedTypes& allowedTypeIds);
        void setAllowedTargetTypeId(const MapFeature::AllowedTypes& allowedTypeIds);

        const MapFeature::AllowedTypes& getAllowedSourceTypeIds() const;
        const MapFeature::AllowedTypes& getAllowedTargetTypeIds() const;
       
        const TypeRef& getSourceTypeId() const;
        const TypeRef& getTargetTypeId() const;

        void setSourceTypeId(const TypeRef& sourceId);
        void setTargetTypeId(const TypeRef& targetId);

        const Type* getSourceType() const;
        const Type* getTargetType() const;

        MapData extractMapData() const;
        void setMapData(const MapData& data);

        unsigned int getNumMapEntries() const;
        const MapProjectEntry& getMapEntry(unsigned int index) const;

        void addMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index);
        void removeMapEntry(unsigned int index);
        void replaceMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index);

        /// Check that the entries types match the source and target ids.
        Result validateNewEntry(const MapEntryData& newEntry, bool isLastEntry) const;

        /// Get the validity of the source type (considered with respect to the allowed source type)
        const Result& getSourceTypeValidity() const;
        /// Get the validity of the target type (considered with respect to the allowed target type)
        const Result& getTargetTypeValidity() const;

        const ProjectContext& getProjectContext() const;

      private:
        const ProjectContext& m_projectContext;
        MapFeature::AllowedTypes m_allowedSourceTypeIds;
        MapFeature::AllowedTypes m_allowedTargetTypeIds;

        TypeRef m_sourceTypeId;
        TypeRef m_targetTypeId;

        Result m_sourceTypeValidity;
        Result m_targetTypeValidity;

        std::vector<std::unique_ptr<MapProjectEntry>> m_mapEntries;
    };
}
