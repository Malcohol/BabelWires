/**
 * A feature which manages a single IntValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/mapFeature.hpp>

#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Map/mapType.hpp>
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>

babelwires::MapFeature::MapFeature(const TypeRef& sourceType, const TypeRef& targetType,
                                     MapEntryData::Kind defaultFallbackKind)
    : SimpleValueFeature(TypeRef(
          MapTypeConstructor::getThisIdentifier(),
          TypeConstructorArguments{{sourceType, targetType},
                                   {EnumValue(MapEntryFallbackKind::getIdentifierFromValue(defaultFallbackKind))}})) {}

const babelwires::MapValue& babelwires::MapFeature::get() const {
    return getValueHolder()->is<MapValue>();
}