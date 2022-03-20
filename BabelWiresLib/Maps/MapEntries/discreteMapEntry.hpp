/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Utilities/hash.hpp>

namespace babelwires {
    /// A map entry with a single source value of discrete type.
    class DiscreteMapEntry : public MapEntryData {
      public:
        Value m_sourceValue;
        Value m_targetValue;
        std::size_t getHash() const override;
        bool operator==(const MapEntry& other) const override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        bool isSourceValid(const TypeSystem& typeSystem, LongIdentifier sourceTypeId) const;
        bool isTargetValid(const TypeSystem& typeSystem, LongIdentifier targetTypeId) const;
    };
} // namespace babelwires
