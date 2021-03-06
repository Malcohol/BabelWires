/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::OneToOneMapEntryData::OneToOneMapEntryData() = default;

babelwires::OneToOneMapEntryData::OneToOneMapEntryData(const OneToOneMapEntryData& other) {
    assert(other.m_sourceValue);
    assert(other.m_targetValue);
    m_sourceValue = other.m_sourceValue->clone();
    m_targetValue = other.m_targetValue->clone();
}

babelwires::OneToOneMapEntryData::OneToOneMapEntryData(OneToOneMapEntryData&& other) {
    m_sourceValue = std::move(other.m_sourceValue);
    m_targetValue = std::move(other.m_targetValue);
}

babelwires::OneToOneMapEntryData::OneToOneMapEntryData(const TypeSystem& typeSystem, LongIdentifier sourceTypeId,
                                                       LongIdentifier targetTypeId) {
    const Type* sourceType = typeSystem.getEntryByIdentifier(sourceTypeId);
    assert(sourceType && "You cannot construct a OneToOneMapEntryData entry with an unknown source type");
    const Type* targetType = typeSystem.getEntryByIdentifier(targetTypeId);
    assert(targetType && "You cannot construct a OneToOneMapEntryData entry with an unknown target type");

    m_sourceValue = sourceType->createValue();
    m_targetValue = targetType->createValue();
}

std::size_t babelwires::OneToOneMapEntryData::getHash() const {
    // "Discrete" - arbitrary discriminator
    return hash::mixtureOf(0xd15c4e7e, m_sourceValue->getHash(), m_targetValue->getHash());
}

bool babelwires::OneToOneMapEntryData::operator==(const MapEntryData& other) const {
    const OneToOneMapEntryData* otherData = other.as<OneToOneMapEntryData>();
    if (!otherData) {
        return false;
    }
    assert(m_sourceValue);
    assert(otherData->m_sourceValue);
    assert(m_targetValue);
    assert(otherData->m_targetValue);
    return (*m_sourceValue == *otherData->m_sourceValue) && (*m_targetValue == *otherData->m_targetValue);
}

const babelwires::Value* babelwires::OneToOneMapEntryData::getSourceValue() const {
    return m_sourceValue.get();
}

void babelwires::OneToOneMapEntryData::setSourceValue(std::unique_ptr<Value> value) {
    m_sourceValue = std::move(value);
}

const babelwires::Value* babelwires::OneToOneMapEntryData::getTargetValue() const {
    return m_targetValue.get();
}

void babelwires::OneToOneMapEntryData::setTargetValue(std::unique_ptr<Value> value) {
    m_targetValue = std::move(value);
}

void babelwires::OneToOneMapEntryData::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(*m_sourceValue, "source");
    serializer.serializeObject(*m_targetValue, "target");
}

void babelwires::OneToOneMapEntryData::deserializeContents(Deserializer& deserializer) {
    // TODO: If refactoring to a constructor, can remove the null handling in == and clone.
    m_sourceValue = deserializer.deserializeObject<Value>("source");
    m_targetValue = deserializer.deserializeObject<Value>("target");
}

void babelwires::OneToOneMapEntryData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_sourceValue->visitIdentifiers(visitor);
    m_targetValue->visitIdentifiers(visitor);
}

void babelwires::OneToOneMapEntryData::visitFilePaths(FilePathVisitor& visitor) {
    m_sourceValue->visitFilePaths(visitor);
    m_targetValue->visitFilePaths(visitor);
}

babelwires::Result babelwires::OneToOneMapEntryData::doValidate(const Type& sourceType, const Type& targetType) const {
    const bool sourceTypeIsValid = m_sourceValue->isValid(sourceType);
    const bool targetTypeIsValid = m_targetValue->isValid(targetType);
    if (!sourceTypeIsValid && !targetTypeIsValid) {
        return "Neither source nor target values are valid.";
    } else if (!sourceTypeIsValid) {
        return "The source value isn't valid.";
    } else if (!targetTypeIsValid) {
        return "The target value isn't valid.";
    }
    return Result::success;
}

babelwires::MapEntryData::Kind babelwires::OneToOneMapEntryData::getKind() const {
    return Kind::OneToOne;
}
