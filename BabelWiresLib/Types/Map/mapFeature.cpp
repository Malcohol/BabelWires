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

babelwires::MapFeature2::MapFeature2(const TypeRef& sourceType, const TypeRef& targetType,
                                     MapEntryData::Kind defaultFallbackKind)
    : m_sourceType(sourceType)
    , m_targetType(targetType)
    , m_defaultFallbackKind(defaultFallbackKind) {}

const babelwires::MapValue& babelwires::MapFeature2::get() const {
    return getValue().is<MapValue>();
}

void babelwires::MapFeature2::doSetToDefault() {
    if (!getTypeRef()) {
        const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*this);
        const MapEntryFallbackKind& fallbackKindEnum = typeSystem.getEntryByType<MapEntryFallbackKind>();
        const babelwires::ShortId kindId = fallbackKindEnum.getIdentifierFromValue(m_defaultFallbackKind);
        setTypeRef(TypeRef(MapTypeConstructor::getThisIdentifier(),
                           TypeConstructorArguments{{m_sourceType, m_targetType}, {EnumValue(kindId)}}));
    }
    SimpleValueFeature::doSetToDefault();
}
