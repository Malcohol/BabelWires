/**
 * A Map defines a how values between two types.
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
    class MapEntry;

    /// 
    class Map : public Serializable, public ProjectVisitable {
      public:
        SERIALIZABLE(Map, "map", void, 1);

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

        unsigned int getNumMapEntries() const;
        const MapEntry& getMapEntry(unsigned int index) const;

        void addMapEntry(std::unique_ptr<MapEntry> newEntry, unsigned int index);
        void removeMapEntry(unsigned int index);

        bool operator==(const Map& other) const;
        bool operator!=(const Map& other) const;

        std::size_t getHash() const;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// Check that the entries types match the source and target ids.
        bool validateNewEntry(const MapEntry& newEntry) const;

      private:
        LongIdentifier m_sourceId;
        LongIdentifier m_targetId;
        std::vector<std::unique_ptr<MapEntry>> m_mapEntries;
    };
}
