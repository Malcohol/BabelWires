/**
 * A TypeConstructor which constructs an int type with a particular range and default.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>

babelwires::MapEntryData::Kind
babelwires::MapTypeConstructor::extractValueArguments(const TypeSystem& typeSystem,
                                                      const std::vector<ValueHolder>& valueArguments) {
    if (valueArguments.size() > 1) {
        throw TypeSystemException() << "MapTypeConstructor expects at most 1 value argument but got "
                                    << valueArguments.size();
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

babelwires::TypeConstructor::TypeConstructorResult
babelwires::MapTypeConstructor::constructType(const TypeSystem& typeSystem, TypeRef newTypeRef,
                                              const TypeConstructorArguments& arguments,
                                              const std::vector<const Type*>& resolvedTypeArguments) const {
    if (arguments.m_typeArguments.size() != 2) {
        throw TypeSystemException() << "MapTypeConstructor expects 2 type arguments but got "
                                    << arguments.m_typeArguments.size();
    }
    babelwires::MapEntryData::Kind kind = extractValueArguments(typeSystem, arguments.m_valueArguments);
    return std::make_unique<ConstructedType<MapType>>(std::move(newTypeRef), arguments.m_typeArguments[0],
                                                      arguments.m_typeArguments[1], kind);
}

babelwires::TypeRef babelwires::MapTypeConstructor::makeTypeRef(TypeRef sourceTypeRef, TypeRef targetTypeRef,
                                                                MapEntryData::Kind fallbackKind) {
    return TypeRef{getThisIdentifier(),
                   TypeConstructorArguments{{std::move(sourceTypeRef), std::move(targetTypeRef)},
                                            {EnumValue(MapEntryFallbackKind::getIdentifierFromValue(fallbackKind))}}};
}
