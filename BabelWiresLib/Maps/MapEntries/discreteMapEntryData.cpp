/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>

#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

std::size_t babelwires::DiscreteMapEntryData::getHash() const {
    // "Discrete" - arbitrary discriminator
    return hash::mixtureOf(0xd15c4e7e, m_sourceValue->getHash(), m_targetValue->getHash());
}

bool babelwires::DiscreteMapEntryData::operator==(const MapEntryData& other) const {
    // TODO return (m_sourceValue == other.m_sourceValue) && (m_targetValue == other.m_targetValue);
    return true;
}

void babelwires::DiscreteMapEntryData::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(*m_sourceValue, "source");
    serializer.serializeObject(*m_targetValue, "target");
}

void babelwires::DiscreteMapEntryData::deserializeContents(Deserializer& deserializer) {
    m_sourceValue = deserializer.deserializeObject<Value>("source");
    m_targetValue = deserializer.deserializeObject<Value>("target");
}

void babelwires::DiscreteMapEntryData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_sourceValue->visitIdentifiers(visitor);
    m_targetValue->visitIdentifiers(visitor);
}

void babelwires::DiscreteMapEntryData::visitFilePaths(FilePathVisitor& visitor) {
    m_sourceValue->visitFilePaths(visitor);
    m_targetValue->visitFilePaths(visitor);
}

bool babelwires::DiscreteMapEntryData::isValid(const Type& sourceType, const Type& targetType) const  {
    return m_sourceValue->isValid(sourceType) && m_targetValue->isValid(targetType);
}
