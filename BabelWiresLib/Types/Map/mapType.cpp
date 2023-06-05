/**
 * MapType is the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/mapType.hpp>

#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

babelwires::MapType::MapType(TypeRef sourceTypeRef, TypeRef targetTypeRef, MapEntryData::Kind defaultFallbackKind)
    : m_sourceTypeRef(std::move(sourceTypeRef))
    , m_targetTypeRef(std::move(targetTypeRef))
    , m_defaultFallbackKind(defaultFallbackKind) {
    assert(MapEntryData::isFallback(defaultFallbackKind) && "Only a fallback kind is expected here");
}

babelwires::NewValueHolder babelwires::MapType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<MapValue>(typeSystem, m_sourceTypeRef, m_targetTypeRef, m_defaultFallbackKind);
}

bool babelwires::MapType::isValidValue(const TypeSystem& typeSystem, const Value& v) const {
    if (const MapValue* const map = v.as<MapValue>()) {
        // Because of the fallback entry, we don't need contravariance here.
        return typeSystem.isRelatedType(map->getSourceTypeRef(), m_sourceTypeRef) &&
               typeSystem.isSubType(map->getTargetTypeRef(), m_targetTypeRef);
    }
    return false;
}

std::string babelwires::MapType::getKind() const {
    return MapValue::serializationType;
}

const babelwires::TypeRef& babelwires::MapType::getSourceTypeRef() const {
    return m_sourceTypeRef;
}
const babelwires::TypeRef& babelwires::MapType::getTargetTypeRef() const {
    return m_targetTypeRef;
}