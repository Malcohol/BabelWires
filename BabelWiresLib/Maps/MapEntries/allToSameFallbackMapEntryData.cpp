/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>

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

babelwires::Result babelwires::AllToSameFallbackMapEntryData::doValidate(const Type& sourceType, const Type& targetType) const {
    if (sourceType.getIdentifier() != targetType.getIdentifier()) {
        return "The source and target types do not match, so an \"All to Same\" mapping can't be used.";
    }
    return {};
}

babelwires::MapEntryData::Kind babelwires::AllToSameFallbackMapEntryData::getKind() const {
    return Kind::AllToSame;
}