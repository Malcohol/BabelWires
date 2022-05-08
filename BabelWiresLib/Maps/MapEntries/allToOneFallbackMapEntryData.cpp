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

babelwires::AllToOneFallbackMapEntryData::AllToOneFallbackMapEntryData() = default;

babelwires::AllToOneFallbackMapEntryData::AllToOneFallbackMapEntryData(const AllToOneFallbackMapEntryData& other) {
    m_targetValue = other.m_targetValue->clone();
}

babelwires::AllToOneFallbackMapEntryData::AllToOneFallbackMapEntryData(AllToOneFallbackMapEntryData&& other) {
    m_targetValue = std::move(other.m_targetValue);
}

const babelwires::Value* babelwires::AllToOneFallbackMapEntryData::getTargetValue() const {
    return m_targetValue.get();
}

void babelwires::AllToOneFallbackMapEntryData::setTargetValue(std::unique_ptr<Value> value) {
    m_targetValue = std::move(value);
}

std::size_t babelwires::AllToOneFallbackMapEntryData::getHash() const {
    // "AllToOne" - arbitrary discriminator
    return hash::mixtureOf(0xa7721, m_targetValue->getHash());
}

bool babelwires::AllToOneFallbackMapEntryData::operator==(const MapEntryData& other) const {
    const AllToOneFallbackMapEntryData* otherData = other.as<AllToOneFallbackMapEntryData>();
    return otherData && (m_targetValue == otherData->m_targetValue);
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

babelwires::Result babelwires::AllToOneFallbackMapEntryData::doValidate(const Type& sourceType, const Type& targetType) const  {
    const bool targetTypeIsValid = m_targetValue->isValid(targetType);
    if (!targetTypeIsValid) {
        return "The target value isn't valid.";
    }
    return {};
}
