/**
 * MapType is the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Map/mapType.hpp>

#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

babelwires::MapType::MapType(TypeRef sourceTypeRef, TypeRef targetTypeRef, MapEntryData::Kind defaultFallbackKind)
    : m_sourceTypeRef(std::move(sourceTypeRef))
    , m_targetTypeRef(std::move(targetTypeRef))
    , m_defaultFallbackKind(defaultFallbackKind) {
    assert(MapEntryData::isFallback(defaultFallbackKind) && "Only a fallback kind is expected here");
}

babelwires::NewValueHolder babelwires::MapType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<MapValue>(typeSystem, m_sourceTypeRef, m_targetTypeRef, m_defaultFallbackKind);
}

bool babelwires::MapType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    if (const MapValue* const map = v.as<MapValue>()) {
        // Because of the fallback entry, we don't need contravariance here.
        return typeSystem.isRelatedType(map->getSourceTypeRef(), m_sourceTypeRef) &&
               typeSystem.isSubType(map->getTargetTypeRef(), m_targetTypeRef);
    }
    return false;
}

std::string babelwires::MapType::getFlavour() const {
    return MapValue::serializationType;
}

const babelwires::TypeRef& babelwires::MapType::getSourceTypeRef() const {
    return m_sourceTypeRef;
}
const babelwires::TypeRef& babelwires::MapType::getTargetTypeRef() const {
    return m_targetTypeRef;
}

std::optional<babelwires::SubtypeOrder> babelwires::MapType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                   const Type& other) const {
    const MapType* const otherMapType = other.as<MapType>();
    if (!otherMapType) {
        return {};
    }
    const SubtypeOrder sourceOrder =
        typeSystem.compareSubtype(m_sourceTypeRef, otherMapType->m_sourceTypeRef);
    if (sourceOrder == SubtypeOrder::IsDisjoint) {
        // Because of the fallback logic, we only exclude disjoint source types here.
        return SubtypeOrder::IsDisjoint;
    }
    const SubtypeOrder targetOrder =
        typeSystem.compareSubtype(m_targetTypeRef, otherMapType->m_targetTypeRef);
    return targetOrder;
}

std::string babelwires::MapType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->is<MapValue>().toString();
}