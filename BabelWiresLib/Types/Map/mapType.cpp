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

babelwires::MapType::MapType(TypeExp sourceTypeExp, TypeExp targetTypeExp, MapEntryData::Kind defaultFallbackKind)
    : m_sourceTypeExp(std::move(sourceTypeExp))
    , m_targetTypeExp(std::move(targetTypeExp))
    , m_defaultFallbackKind(defaultFallbackKind) {
    assert(MapEntryData::isFallback(defaultFallbackKind) && "Only a fallback kind is expected here");
}

babelwires::NewValueHolder babelwires::MapType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<MapValue>(typeSystem, m_sourceTypeExp, m_targetTypeExp, m_defaultFallbackKind);
}

bool babelwires::MapType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    if (const MapValue* const map = v.as<MapValue>()) {
        // Because of the fallback entry, we don't need contravariance here.
        return typeSystem.isRelatedType(map->getSourceTypeExp(), m_sourceTypeExp) &&
               typeSystem.isSubType(map->getTargetTypeExp(), m_targetTypeExp);
    }
    return false;
}

std::string babelwires::MapType::getFlavour() const {
    return MapValue::serializationType;
}

const babelwires::TypeExp& babelwires::MapType::getSourceTypeExp() const {
    return m_sourceTypeExp;
}
const babelwires::TypeExp& babelwires::MapType::getTargetTypeExp() const {
    return m_targetTypeExp;
}

std::optional<babelwires::SubtypeOrder> babelwires::MapType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                   const Type& other) const {
    const MapType* const otherMapType = other.as<MapType>();
    if (!otherMapType) {
        return {};
    }
    const SubtypeOrder sourceOrder =
        typeSystem.compareSubtype(m_sourceTypeExp, otherMapType->m_sourceTypeExp);
    if (sourceOrder == SubtypeOrder::IsDisjoint) {
        // Because of the fallback logic, we only exclude disjoint source types here.
        return SubtypeOrder::IsDisjoint;
    }
    const SubtypeOrder targetOrder =
        typeSystem.compareSubtype(m_targetTypeExp, otherMapType->m_targetTypeExp);
    return targetOrder;
}

std::string babelwires::MapType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->is<MapValue>().toString();
}