/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>

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

babelwires::OneToOneMapEntryData::OneToOneMapEntryData(const TypeSystem& typeSystem, const TypeRef& sourceTypeRef,
                                                       const TypeRef& targetTypeRef) {
    const Type* sourceType = sourceTypeRef.tryResolve(typeSystem);
    assert(sourceType && "You cannot construct a OneToOneMapEntryData entry with an unknown source type");
    const Type* targetType = targetTypeRef.tryResolve(typeSystem);
    assert(targetType && "You cannot construct a OneToOneMapEntryData entry with an unknown target type");

    m_sourceValue = sourceType->createValue(typeSystem);
    assert(m_sourceValue->tryGetAsEditableValue() && "MapEntries must contain editable data");
    m_targetValue = targetType->createValue(typeSystem);
    assert(m_targetValue->tryGetAsEditableValue() && "MapEntries must contain editable data");
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

const babelwires::EditableValueHolder* babelwires::OneToOneMapEntryData::tryGetSourceValue() const {
    return &m_sourceValue;
}

void babelwires::OneToOneMapEntryData::setSourceValue(EditableValueHolder value) {
    assert(value->tryGetAsEditableValue() && "MapEntries must contain editable data");
    m_sourceValue = std::move(value);
}

const babelwires::EditableValueHolder* babelwires::OneToOneMapEntryData::tryGetTargetValue() const {
    return &m_targetValue;
}

void babelwires::OneToOneMapEntryData::setTargetValue(EditableValueHolder value) {
    assert(value->tryGetAsEditableValue() && "MapEntries must contain editable data");
    m_targetValue = std::move(value);
}

void babelwires::OneToOneMapEntryData::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(m_sourceValue->getAsEditableValue(), "source");
    serializer.serializeObject(m_targetValue->getAsEditableValue(), "target");
}

void babelwires::OneToOneMapEntryData::deserializeContents(Deserializer& deserializer) {
    // TODO: If refactoring to a constructor, can remove the null handling in == and clone.
    m_sourceValue = uniquePtrCast<Value>(deserializer.deserializeObject<EditableValue>("source"));
    m_targetValue = uniquePtrCast<Value>(deserializer.deserializeObject<EditableValue>("target"));
}

void babelwires::OneToOneMapEntryData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_sourceValue.visitIdentifiers(visitor);
    m_targetValue.visitIdentifiers(visitor);
}

void babelwires::OneToOneMapEntryData::visitFilePaths(FilePathVisitor& visitor) {
    m_sourceValue.visitFilePaths(visitor);
    m_targetValue.visitFilePaths(visitor);
}

babelwires::Result babelwires::OneToOneMapEntryData::doValidate(const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType) const {
    const bool sourceTypeIsValid = sourceType.isValidValue(typeSystem, *m_sourceValue);
    const bool targetTypeIsValid = targetType.isValidValue(typeSystem, *m_targetValue);
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
    return Kind::One21;
}
