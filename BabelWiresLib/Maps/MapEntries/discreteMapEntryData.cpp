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
    const DiscreteMapEntryData* otherData = other.as<DiscreteMapEntryData>();
    return otherData && (m_sourceValue == otherData->m_sourceValue) && (m_targetValue == otherData->m_targetValue);
}

const babelwires::Value* babelwires::DiscreteMapEntryData::getSourceValue() const {
    return m_sourceValue.get();
}

void babelwires::DiscreteMapEntryData::setSourceValue(std::unique_ptr<Value> value) {
    m_sourceValue = std::move(value);
}

const babelwires::Value* babelwires::DiscreteMapEntryData::getTargetValue() const {
    return m_targetValue.get();
}

void babelwires::DiscreteMapEntryData::setTargetValue(std::unique_ptr<Value> value) {
    m_targetValue = std::move(value);
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
