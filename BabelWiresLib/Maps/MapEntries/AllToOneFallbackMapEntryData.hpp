/**
 * FallbackMapEntries are the last entry in a map, and define what happens to previously unmapped source values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/MapEntries/fallbackMapEntryData.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

namespace babelwires {

    class AllToOneFallbackMapEntryData : public FallbackMapEntryData<void, SOURCE_AND_TARGET_TYPE> {
      public:
        Value m_targetValue;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        bool isSourceValid(const TypeSystem& typeSystem, LongIdentifier sourceTypeId) const override;
        bool isTargetValid(const TypeSystem& typeSystem, LongIdentifier targetTypeId) const override;
    };
} // namespace babelwires
