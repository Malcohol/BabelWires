/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/MapEntries/discreteMapEntry.hpp>

std::size_t babelwires::DiscreteMapEntry::getHash() const {
    // "Discrete" - arbitrary discriminator
    return hash::mixtureOf(0xd15c4e7e, m_sourceValue.getHash(), m_targetValue.getHash());
}

bool babelwires::DiscreteMapEntry::operator==(const MapEntry& other) const {
    // TODO return (m_sourceValue == other.m_sourceValue) && (m_targetValue == other.m_targetValue);
    return true;
}

void babelwires::DiscreteMapEntry::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(m_sourceValue, "source");
    serializer.serializeObject(m_targetValue, "target");
}

void babelwires::DiscreteMapEntry::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeObject(m_sourceValue, "source");
    deserializer.deserializeObject(m_targetValue, "target");
}

void babelwires::DiscreteMapEntry::visitIdentifiers(IdentifierVisitor& visitor) {
    m_sourceValue.visit(visitor);
    m_targetValue.visit(visitor);
}

void babelwires::DiscreteMapEntry::visitFilePaths(FilePathVisitor& visitor) {
    m_sourceValue.visit(visitor);
    m_targetValue.visit(visitor);
}
