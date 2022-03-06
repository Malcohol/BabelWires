/**
 * MapEntries define a relationship between values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/projectVisitable.hpp"
#include "BabelWiresLib/Maps/typeSystem.hpp"

#include "Common/Cloning/cloneable.hpp"

#include <Common/types.hpp>

namespace babelwires {
    class MapEntryData;

    class MapEntry : public Cloneable {
      public:
        CLONEABLE(MapEntry);
        MapEntry(std::unique_ptr<MapEntryData> data);
        MapEntry(std::unique_ptr<MapEntryData> data, std::string reasonForFailure);
        MapEntry(const MapEntry& other);
        virtual ~MapEntry();
        
        const MapEntryData& getData() const;
      public:
        std::unique_ptr<MapEntryData> m_data;
        std::string m_reasonForFailure;
    };
} // namespace babelwires

