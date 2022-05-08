/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/discreteMapEntryData.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::DiscreteMapEntryData::DiscreteMapEntryData() = default;

babelwires::DiscreteMapEntryData::DiscreteMapEntryData(const DiscreteMapEntryData& other) {
    m_sourceValue = other.m_sourceValue->clone();
    m_targetValue = other.m_targetValue->clone();
}

babelwires::DiscreteMapEntryData::DiscreteMapEntryData(DiscreteMapEntryData&& other) {
    m_sourceValue = std::move(other.m_sourceValue);
    m_targetValue = std::move(other.m_targetValue);
}

babelwires::DiscreteMapEntryData::DiscreteMapEntryData(const ProjectContext& context, LongIdentifier sourceTypeId,
                                                       LongIdentifier targetTypeId) {
    const Type* sourceType = context.m_typeSystem.getEntryByIdentifier(sourceTypeId);
    assert(sourceType && "You cannot construct a DiscreteMapEntryData entry with an unknown source type");
    const Type* targetType = context.m_typeSystem.getEntryByIdentifier(targetTypeId);
    assert(targetType && "You cannot construct a DiscreteMapEntryData entry with an unknown target type");

    m_sourceValue = sourceType->createValue();
    m_targetValue = targetType->createValue();
}

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

babelwires::Result babelwires::DiscreteMapEntryData::validate(const Type& sourceType,
                                                                              const Type& targetType) const {
    const bool sourceTypeIsValid = m_sourceValue->isValid(sourceType);
    const bool targetTypeIsValid = m_targetValue->isValid(targetType);
    if (!sourceTypeIsValid && !targetTypeIsValid) {
        return "Neither source nor target values are valid.";
    } else if (!sourceTypeIsValid) {
        return "The source value isn't valid.";
    } else if (!targetTypeIsValid) {
        return "The target value isn't valid.";
    }
    return {};
}
