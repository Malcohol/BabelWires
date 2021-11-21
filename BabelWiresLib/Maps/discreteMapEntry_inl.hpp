
template <typename SOURCE_TYPE, typename TARGET_TYPE>
std::size_t babelwires::DiscreteMapEntry<SOURCE_TYPE, TARGET_TYPE>::getHash() const {
    return hash::mixtureOf(m_sourceValue, m_targetValue);
}

template <typename SOURCE_TYPE, typename TARGET_TYPE>
bool babelwires::DiscreteMapEntry::operator==(const MapEntry& other) const {
    return (m_sourceValue == other.m_sourceValue) && (m_targetValue == other.m_targetValue);
}
