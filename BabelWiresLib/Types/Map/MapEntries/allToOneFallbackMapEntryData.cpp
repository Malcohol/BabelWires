/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>

#include <BabelWiresLib/TypeSystem/editableValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <Common/Serialization/serializer.hpp>
#include <Common/Serialization/deserializer.hpp>

babelwires::AllToOneFallbackMapEntryData::AllToOneFallbackMapEntryData() = default;

babelwires::AllToOneFallbackMapEntryData::AllToOneFallbackMapEntryData(const TypeSystem& typeSystem, const TypeExp& targetTypeRef) {
    const TypePtr targetType = targetTypeRef.tryResolve(typeSystem);
    assert(targetType && "You cannot construct a OneToOneMapEntryData entry with an unknown target type");
    m_targetValue = targetType->createValue(typeSystem);
    assert(m_targetValue->tryGetAsEditableValue() && "MapEntries must contain editable data");
}

babelwires::AllToOneFallbackMapEntryData::AllToOneFallbackMapEntryData(const AllToOneFallbackMapEntryData& other) {
    assert(other.m_targetValue);
    m_targetValue = other.m_targetValue->clone();
}

babelwires::AllToOneFallbackMapEntryData::AllToOneFallbackMapEntryData(AllToOneFallbackMapEntryData&& other) {
    m_targetValue = std::move(other.m_targetValue);
}

const babelwires::ValueHolder* babelwires::AllToOneFallbackMapEntryData::tryGetTargetValue() const {
    return &m_targetValue;
}

void babelwires::AllToOneFallbackMapEntryData::setTargetValue(ValueHolder value) {
    assert(value->tryGetAsEditableValue() && "MapEntries must contain editable data");
    m_targetValue = std::move(value);
}

std::size_t babelwires::AllToOneFallbackMapEntryData::getHash() const {
    // "AllToOne" - arbitrary discriminator
    return hash::mixtureOf(0xa7721, m_targetValue->getHash());
}

bool babelwires::AllToOneFallbackMapEntryData::operator==(const MapEntryData& other) const {
    const AllToOneFallbackMapEntryData* otherData = other.as<AllToOneFallbackMapEntryData>();
    if (!otherData) {
        return false;
    }
    assert(m_targetValue);
    assert(otherData->m_targetValue);
    return *m_targetValue == *otherData->m_targetValue;
}

void babelwires::AllToOneFallbackMapEntryData::serializeContents(Serializer& serializer) const {
    serializer.serializeObject(m_targetValue->getAsEditableValue(), "target");
}

void babelwires::AllToOneFallbackMapEntryData::deserializeContents(Deserializer& deserializer) {
    m_targetValue = uniquePtrCast<Value>(deserializer.deserializeObject<EditableValue>("target"));
}

void babelwires::AllToOneFallbackMapEntryData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_targetValue.visitIdentifiers(visitor);
}

void babelwires::AllToOneFallbackMapEntryData::visitFilePaths(FilePathVisitor& visitor) {
    m_targetValue.visitFilePaths(visitor);
}

babelwires::Result babelwires::AllToOneFallbackMapEntryData::doValidate(const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType) const  {
    const bool targetTypeIsValid = targetType.isValidValue(typeSystem, *m_targetValue);
    if (!targetTypeIsValid) {
        return "The target value isn't valid.";
    }
    return Result::success;
}

babelwires::MapEntryData::Kind babelwires::AllToOneFallbackMapEntryData::getKind() const {
    return Kind::All21;
}
