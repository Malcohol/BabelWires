/**
 * The MapProject carries an enriched version of the data in a MapValue and has support for editing.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Serialization/serializable.hpp>
#include <Common/Utilities/result.hpp>

#include <vector>
#include <memory>

namespace babelwires {
    class MapProjectEntry;
    class MapEntryData;
    struct ProjectContext;

    /// The MapProject carries an enriched version of the data in a MapValue and has support for editing.
    class MapProject {
      public:
        MapProject(const ProjectContext& projectContext);
        virtual ~MapProject();

        struct AllowedTypes {
            std::vector<TypeRef> m_typeRefs;
            unsigned int m_indexOfDefault = 0;

            bool isRelatedToSome(const TypeSystem& typeSystem, const TypeRef& type) const;
            bool isSubtypeOfSome(const TypeSystem& typeSystem, const TypeRef& type) const;
            const TypeRef& getDefaultTypeRef() const { return m_typeRefs[m_indexOfDefault]; }
        };

        void setAllowedSourceTypeRefs(const AllowedTypes& allowedTypes);
        void setAllowedTargetTypeRefs(const AllowedTypes& allowedTypes);

        const AllowedTypes& getAllowedSourceTypeRefs() const;
        const AllowedTypes& getAllowedTargetTypeRefs() const;
       
        const TypeRef& getSourceTypeRef() const;
        const TypeRef& getTargetTypeRef() const;

        void setSourceTypeRef(const TypeRef& sourceId);
        void setTargetTypeRef(const TypeRef& targetId);

        const Type* getSourceType() const;
        const Type* getTargetType() const;

        MapValue extractMapValue() const;
        void setMapValue(const MapValue& data);

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
        AllowedTypes m_allowedSourceTypeRefs;
        AllowedTypes m_allowedTargetTypeRefs;

        TypeRef m_sourceTypeRef;
        TypeRef m_targetTypeRef;

        Result m_sourceTypeValidity;
        Result m_targetTypeValidity;

        std::vector<std::unique_ptr<MapProjectEntry>> m_mapEntries;
    };
}
