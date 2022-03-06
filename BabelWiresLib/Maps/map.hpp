/**
 * A Map defines a how values between two types.
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
    class MapEntry;
    class MapEntryData;

    /// 
    class Map {
      public:
        Map();
        Map(const Map& other);
        Map(Map&& other);
        Map& operator=(const Map& other);
        Map& operator=(Map&& other);
        virtual ~Map();

        LongIdentifier getSourceId() const;
        LongIdentifier getTargetId() const;

        void setSourceId(LongIdentifier sourceId);
        void setTargetId(LongIdentifier targetId);

        MapData extractMapData() const;
        void setMapData(const MapData& data);

        unsigned int getNumMapEntries() const;
        const MapEntry& getMapEntry(unsigned int index) const;

        void addMapEntry(std::unique_ptr<MapEntryData> newEntry, unsigned int index);
        void removeMapEntry(unsigned int index);

        bool operator==(const Map& other) const;
        bool operator!=(const Map& other) const;

        /// Check that the entries types match the source and target ids.
        bool validateNewEntry(const MapEntryData& newEntry) const;

      private:
        LongIdentifier m_sourceId;
        LongIdentifier m_targetId;
        std::vector<std::unique_ptr<MapEntry>> m_mapEntries;
    };
}
