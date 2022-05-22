/**
 * MapEntryData define a relationship between source and target values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Maps/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Maps/MapEntries/allToSameFallbackMapEntryData.hpp>

babelwires::MapEntryData::~MapEntryData() = default;

babelwires::Result babelwires::MapEntryData::validate(const TypeSystem& typeSystem, LongIdentifier sourceTypeId, LongIdentifier targetTypeId, bool isLastEntry) const {
    const Type *const sourceType = typeSystem.getEntryByIdentifier(sourceTypeId);
    if (!sourceType) {
        return "The source type is not recognized";
    }
    const Type *const targetType = typeSystem.getEntryByIdentifier(targetTypeId);
    if (!targetType) {
        return "The target type is not recognized";
    }
    //if (isLastEntry != (this->as<FallbackMapEntryData>() != nullptr)) {
    //   return isLastEntry ? "The last entry must be a fallback entry" : "A fallback entry can only be at the end of a map";
    //}
    return doValidate(*sourceType, *targetType);
}

std::unique_ptr<babelwires::MapEntryData> babelwires::MapEntryData::create(const TypeSystem& typeSystem, LongIdentifier sourceTypeId, LongIdentifier targetTypeId, Kind kind) {
    switch (kind) {
        case Kind::OneToOne:
            return std::make_unique<OneToOneMapEntryData>(typeSystem, sourceTypeId, targetTypeId);
        case Kind::AllToOne:
            return std::make_unique<AllToOneFallbackMapEntryData>(typeSystem, targetTypeId);
        case Kind::AllToSame:
            return std::make_unique<AllToSameFallbackMapEntryData>();
    }
    assert(false && "Invalid kind");
    return {};
}

bool babelwires::MapEntryData::isFallback(Kind kind) {
    return (kind == Kind::AllToOne) || (kind == Kind::AllToSame);
}

std::string babelwires::MapEntryData::getKindName(Kind kind) {
    switch (kind) {
        case Kind::OneToOne:
            return "One to One";
        case Kind::AllToOne:
            return "All to One";
        case Kind::AllToSame:
            return "All to Same";
    }
    assert(false && "Invalid kind");
    return {};
}
