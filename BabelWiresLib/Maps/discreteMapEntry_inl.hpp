
template <typename SOURCE_TYPE, typename TARGET_TYPE>
std::size_t babelwires::DiscreteMapEntry<SOURCE_TYPE, TARGET_TYPE>::getHash() const {
    return hash::mixtureOf(m_sourceValue, m_targetValue);
}

template <typename SOURCE_TYPE, typename TARGET_TYPE>
bool babelwires::DiscreteMapEntry::operator==(const MapEntry& other) const {
    return (m_sourceValue == other.m_sourceValue) && (m_targetValue == other.m_targetValue);
}

template <typename SOURCE_TYPE, typename TARGET_TYPE>
void babelwires::DiscreteMapEntry::serializeContents(Serializer& serializer) const {
    // TODO: Don't serialize enum index!!!
    serializer.serializeValue("source", m_sourceValue);
    serializer.serializeValue("target", m_targetValue);
}

template <typename SOURCE_TYPE, typename TARGET_TYPE>
void babelwires::DiscreteMapEntry::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("source", m_sourceValue);
    deserializer.deserializeValue("target", m_targetValue);
}

template <typename SOURCE_TYPE, typename TARGET_TYPE>
void babelwires::DiscreteMapEntry::visitIdentifiers(IdentifierVisitor& visitor) {
    visitor(m_sourceValue);
    visitor(m_targetValue);
}

template <typename SOURCE_TYPE, typename TARGET_TYPE>
void babelwires::DiscreteMapEntry::visitFilePaths(FilePathVisitor& visitor) {}
