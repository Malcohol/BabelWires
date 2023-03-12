/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

std::size_t babelwires::AllToSameFallbackMapEntryData::getHash() const {
    // "Id Fallback" (an arbitrary hash value)
    return 0x1DFA77BC;
}

bool babelwires::AllToSameFallbackMapEntryData::operator==(const MapEntryData& other) const {
    return other.as<AllToSameFallbackMapEntryData>();
}

void babelwires::AllToSameFallbackMapEntryData::serializeContents(Serializer& serializer) const {
}

void babelwires::AllToSameFallbackMapEntryData::deserializeContents(Deserializer& deserializer) {
}

void babelwires::AllToSameFallbackMapEntryData::visitIdentifiers(IdentifierVisitor& visitor) {
}

void babelwires::AllToSameFallbackMapEntryData::visitFilePaths(FilePathVisitor& visitor) {
}

babelwires::Result babelwires::AllToSameFallbackMapEntryData::doValidate(const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType) const {
    if (!typeSystem.isSubType(sourceType.getTypeRef(), targetType.getTypeRef())) {
        return "An \"All to Same\" mapping can't be used when the source type is not a subtype of the target type.";
    }
    return Result::success;
}

babelwires::MapEntryData::Kind babelwires::AllToSameFallbackMapEntryData::getKind() const {
    return Kind::AllToSame;
}
