/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/identityFallbackMapEntryData.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

std::size_t babelwires::IdentityFallbackMapEntryData::getHash() const {
    // "Id Fallback" (an arbitrary hash value)
    return 0x1DFA77BC;
}

bool babelwires::IdentityFallbackMapEntryData::operator==(const MapEntryData& other) const {
    // TODO return (m_sourceValue == other.m_sourceValue) && (m_targetValue == other.m_targetValue);
    return true;
}

void babelwires::IdentityFallbackMapEntryData::serializeContents(Serializer& serializer) const {
}

void babelwires::IdentityFallbackMapEntryData::deserializeContents(Deserializer& deserializer) {
}

void babelwires::IdentityFallbackMapEntryData::visitIdentifiers(IdentifierVisitor& visitor) {
}

void babelwires::IdentityFallbackMapEntryData::visitFilePaths(FilePathVisitor& visitor) {
}

bool babelwires::IdentityFallbackMapEntryData::isValid(const Type& sourceType, const Type& targetType) const {
    return sourceType.getIdentifier() == targetType.getIdentifier();
}
