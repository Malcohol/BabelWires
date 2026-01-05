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

babelwires::MapType::MapType(const TypeSystem& typeSystem, TypeExp sourceTypeExp, TypeExp targetTypeExp, MapEntryData::Kind defaultFallbackKind)
    : m_sourceType(sourceTypeExp.resolve(typeSystem))
    , m_targetType(targetTypeExp.resolve(typeSystem))
    , m_defaultFallbackKind(defaultFallbackKind) {
    assert(MapEntryData::isFallback(defaultFallbackKind) && "Only a fallback kind is expected here");
}

babelwires::MapType::MapType(TypePtr sourceType, TypePtr targetType, MapEntryData::Kind defaultFallbackKind)
    : m_sourceType(std::move(sourceType))
    , m_targetType(std::move(targetType))
    , m_defaultFallbackKind(defaultFallbackKind) {
    assert(MapEntryData::isFallback(defaultFallbackKind) && "Only a fallback kind is expected here");
}

babelwires::NewValueHolder babelwires::MapType::createValue(const TypeSystem& typeSystem) const {
    return ValueHolder::makeValue<MapValue>(typeSystem, m_sourceType, m_targetType, m_defaultFallbackKind);
}

bool babelwires::MapType::visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const {
    // Note: Map is not currently treated as a compound, so we don't call the visitor on its entries.
    if (const MapValue* const map = v.as<MapValue>()) {
        // Because of the fallback entry, we don't need contravariance here.
        return typeSystem.isRelatedType(map->getSourceTypeExp(), m_sourceType->getTypeExp()) &&
               typeSystem.isSubType(map->getTargetTypeExp(), m_targetType->getTypeExp());
    }
    return false;
}

std::string babelwires::MapType::getFlavour() const {
    return MapValue::serializationType;
}

babelwires::TypeExp babelwires::MapType::getSourceTypeExp() const {
    return m_sourceType->getTypeExp();
}
babelwires::TypeExp babelwires::MapType::getTargetTypeExp() const {
    return m_targetType->getTypeExp();
}

std::optional<babelwires::SubtypeOrder> babelwires::MapType::compareSubtypeHelper(const TypeSystem& typeSystem,
                                                                   const Type& other) const {
    const MapType* const otherMapType = other.as<MapType>();
    if (!otherMapType) {
        return {};
    }
    const SubtypeOrder sourceOrder =
        typeSystem.compareSubtype(m_sourceType->getTypeExp(), otherMapType->m_sourceType->getTypeExp());
    if (sourceOrder == SubtypeOrder::IsDisjoint) {
        // Because of the fallback logic, we only exclude disjoint source types here.
        return SubtypeOrder::IsDisjoint;
    }
    const SubtypeOrder targetOrder =
        typeSystem.compareSubtype(m_targetType->getTypeExp(), otherMapType->m_targetType->getTypeExp());
    return targetOrder;
}

std::string babelwires::MapType::valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const { 
    return v->is<MapValue>().toString();
}