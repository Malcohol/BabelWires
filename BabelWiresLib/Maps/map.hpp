/**
 * A Map defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>

#include <vector>
#include <memory>

namespace babelwires {
    class MapEntry;

    // TODO
    LongIdentifier getIntTypeId();

    /// 
    class Map {
      public:
        Map();
        Map(Map&& other);
        virtual ~Map();

        LongIdentifier getSourceId() const;
        LongIdentifier getTargetId() const;

        void setSourceId(LongIdentifier sourceId);
        void setTargetId(LongIdentifier targetId);

        void addMapEntry(std::unique_ptr<MapEntry> indexToValue);

        bool operator==(const Map& other) const;
        bool operator!=(const Map& other) const;

        std::size_t getHash() const;

      private:
        LongIdentifier m_sourceId;
        LongIdentifier m_targetId;
        std::vector<std::unique_ptr<MapEntry>> m_mapEntries;
    };
}
