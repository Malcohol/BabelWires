/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>

#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

std::size_t babelwires::AllToOneFallbackMapEntryData::getHash() const {
    // "AllToOne" - arbitrary discriminator
    return hash::mixtureOf(0xa7721, m_targetValue->getHash());
}

bool babelwires::AllToOneFallbackMapEntryData::operator==(const MapEntryData& other) const {
    // TODO return (m_sourceValue == other.m_sourceValue) && (m_targetValue == other.m_targetValue);
    return true;
}

void babelwires::AllToOneFallbackMapEntryData::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(*m_targetValue, "target");
}

void babelwires::AllToOneFallbackMapEntryData::deserializeContents(Deserializer& deserializer) {
    m_targetValue = deserializer.deserializeObject<Value>("target");
}

void babelwires::AllToOneFallbackMapEntryData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_targetValue->visitIdentifiers(visitor);
}

void babelwires::AllToOneFallbackMapEntryData::visitFilePaths(FilePathVisitor& visitor) {
    m_targetValue->visitFilePaths(visitor);
}

bool babelwires::AllToOneFallbackMapEntryData::isValid(const Type& sourceType, const Type& targetType) const  {
    return m_targetValue->isValid(targetType);
}

