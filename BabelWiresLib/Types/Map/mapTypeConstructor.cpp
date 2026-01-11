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

    if (const EnumValue* enumValue = valueArguments[0]->tryAs<EnumValue>()) {
        const auto mapEntryFallbackKind = typeSystem.getRegisteredType<MapEntryFallbackKind>();
        return mapEntryFallbackKind->getValueFromIdentifier(enumValue->get());
    } else {
        throw TypeSystemException() << "Value argument 0 given to MapTypeConstructor was not a MapEntryFallbackKind";
    }
}

babelwires::TypePtr
babelwires::MapTypeConstructor::constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                              const TypeConstructorArguments& arguments,
                                              const std::vector<TypePtr>& resolvedTypeArguments) const {
    if (arguments.getTypeArguments().size() != 2) {
        throw TypeSystemException() << "MapTypeConstructor expects 2 type arguments but got "
                                    << arguments.getTypeArguments().size();
    }
    babelwires::MapEntryData::Kind kind = extractValueArguments(typeSystem, arguments.getValueArguments());
    return makeType<MapType>(std::move(newTypeExp), resolvedTypeArguments[0], resolvedTypeArguments[1], kind);
}

babelwires::TypeExp babelwires::MapTypeConstructor::makeTypeExp(TypeExp sourceTypeExp, TypeExp targetTypeExp,
                                                                MapEntryData::Kind fallbackKind) {
    return TypeExp{getThisIdentifier(),
                   TypeConstructorArguments{{std::move(sourceTypeExp), std::move(targetTypeExp)},
                                            {EnumValue(MapEntryFallbackKind::getIdentifierFromValue(fallbackKind))}}};
}
