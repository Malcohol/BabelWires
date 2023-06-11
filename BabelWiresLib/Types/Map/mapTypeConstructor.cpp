/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>

#include <BabelWiresLib/Types/Map/mapType.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>

babelwires::MapEntryData::Kind
babelwires::MapTypeConstructor::extractValueArguments(const TypeSystem& typeSystem,
                                                      const std::vector<EditableValueHolder>& valueArguments) {
    if (valueArguments.size() > 1) {
        throw TypeSystemException() << "MapTypeConstructor expects at most 1 value argument but got " << valueArguments.size();
    } else if (valueArguments.size() == 0) {
        return babelwires::MapEntryData::Kind::All21;
    }

    if (const EnumValue* enumValue = valueArguments[0]->as<EnumValue>()) {
        const MapEntryFallbackKind& mapEntryFallbackKind = typeSystem.getEntryByType<MapEntryFallbackKind>();
        return mapEntryFallbackKind.getValueFromIdentifier(enumValue->get());
    } else {
        throw TypeSystemException() << "Value argument 0 given to MapTypeConstructor was not a MapEntryFallbackKind";
    }
}

std::unique_ptr<babelwires::Type>
babelwires::MapTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                              const std::vector<const Type*>& typeArguments,
                                              const std::vector<EditableValueHolder>& valueArguments) const {
    if (typeArguments.size() != 2) {
        throw TypeSystemException() << "MapTypeConstructor expects 2 type arguments but got " << typeArguments.size();
    }
    babelwires::MapEntryData::Kind kind = extractValueArguments(typeSystem, valueArguments);
    return std::make_unique<ConstructedType<MapType>>(std::move(newTypeRef), typeArguments[0]->getTypeRef(),
                                                      typeArguments[1]->getTypeRef(), kind);
}

