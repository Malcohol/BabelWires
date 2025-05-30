/**
 * MapEntryData define a relationship between source and target values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>

ENUM_DEFINE_ENUM_VALUE_SOURCE(babelwires::MapEntryFallbackKind, BW_MAP_ENTRY_FALLBACK_KIND);

babelwires::MapEntryFallbackKind::MapEntryFallbackKind()
    : EnumType(getStaticValueSet(), 0) {}

babelwires::MapEntryData::~MapEntryData() = default;

babelwires::Result babelwires::MapEntryData::validate(const TypeSystem& typeSystem, const TypeRef& sourceTypeRef,
                                                      const TypeRef& targetTypeRef, bool isLastEntry) const {
    const Type* const sourceType = sourceTypeRef.tryResolve(typeSystem);
    if (!sourceType) {
        return "The source type is not recognized";
    }
    const Type* const targetType = targetTypeRef.tryResolve(typeSystem);
    if (!targetType) {
        return "The target type is not recognized";
    }
    if (isLastEntry != isFallback(getKind())) {
        return isLastEntry ? "The last entry must be a fallback entry"
                           : "A fallback entry can only be at the end of a map";
    }
    return doValidate(typeSystem, *sourceType, *targetType);
}

std::unique_ptr<babelwires::MapEntryData> babelwires::MapEntryData::create(const TypeSystem& typeSystem,
                                                                           const TypeRef& sourceTypeRef,
                                                                           const TypeRef& targetTypeRef, Kind kind) {
    switch (kind) {
        case Kind::One21:
            return std::make_unique<OneToOneMapEntryData>(typeSystem, sourceTypeRef, targetTypeRef);
        case Kind::All21:
            return std::make_unique<AllToOneFallbackMapEntryData>(typeSystem, targetTypeRef);
        case Kind::All2Sm:
            return std::make_unique<AllToSameFallbackMapEntryData>();
    }
    assert(false && "Invalid kind");
    return {};
}

bool babelwires::MapEntryData::isFallback(Kind kind) {
    return (kind == Kind::All21) || (kind == Kind::All2Sm);
}

std::string babelwires::MapEntryData::getKindName(Kind kind) {
    switch (kind) {
        case Kind::One21:
            return "One to One";
        case Kind::All21:
            return "All to One";
        case Kind::All2Sm:
            return "All to Same";
    }
    assert(false && "Invalid kind");
    return {};
}

const babelwires::EditableValueHolder& babelwires::MapEntryData::getSourceValue() const {
    const auto* sourceValue = tryGetSourceValue();
    assert(sourceValue && "Source value expected"); 
    return *sourceValue;
}

const babelwires::EditableValueHolder* babelwires::MapEntryData::tryGetSourceValue() const {
    return nullptr;
}

void babelwires::MapEntryData::setSourceValue(EditableValueHolder value) {
    assert(false && "No source value or not overridden");
}

const babelwires::EditableValueHolder& babelwires::MapEntryData::getTargetValue() const {
    const auto* targetValue = tryGetTargetValue();
    assert(targetValue && "Target value expected"); 
    return *targetValue;
}

const babelwires::EditableValueHolder* babelwires::MapEntryData::tryGetTargetValue() const {
    return nullptr;
}

void babelwires::MapEntryData::setTargetValue(EditableValueHolder value) {
    assert(false && "No target value or not overridden");
}
