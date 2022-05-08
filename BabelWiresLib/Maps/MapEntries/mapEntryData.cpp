/**
 * MapEntryData define a relationship between source and target values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Maps/MapEntries/mapEntryData.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

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
