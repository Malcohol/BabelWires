/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

#include <algorithm>
namespace {
    void insertTypeId(babelwires::TypeSystem::TypeIdSet& typeIds, const babelwires::LongIdentifier& typeId) {
        auto it = std::upper_bound(typeIds.begin(), typeIds.end(), typeId);
        typeIds.insert(it, typeId);
    }
} // namespace

babelwires::TypeSystem::~TypeSystem() = default;

const babelwires::Type* babelwires::TypeSystem::tryGetPrimitiveType(LongIdentifier id) const {
    auto it = m_primitiveTypeRegistry.find(id);
    if (it != m_primitiveTypeRegistry.end()) {
        return std::get<0>(it->second).get();
    }
    return nullptr;
}

const babelwires::Type& babelwires::TypeSystem::getPrimitiveType(LongIdentifier id) const {
    auto it = m_primitiveTypeRegistry.find(id);
    assert((it != m_primitiveTypeRegistry.end()) && "Primitive Type not registered in type system");
    return *std::get<0>(it->second);
}

babelwires::Type* babelwires::TypeSystem::addPrimitiveType(LongIdentifier typeId, VersionNumber version,
                                                           std::unique_ptr<Type> newType) {
    auto addResult = m_primitiveTypeRegistry.emplace(
        std::pair<LongIdentifier, PrimitiveTypeInfo>{typeId, PrimitiveTypeInfo{std::move(newType), version}});
    assert(addResult.second && "Type with that identifier already registered");
    return std::get<0>(addResult.first->second).get();
}

const babelwires::TypeConstructor* babelwires::TypeSystem::tryGetTypeConstructor(LongIdentifier id) const {
    auto it = m_typeConstructorRegistry.find(id);
    if (it != m_typeConstructorRegistry.end()) {
        return std::get<0>(it->second).get();
    }
    return nullptr;
}

const babelwires::TypeConstructor& babelwires::TypeSystem::getTypeConstructor(LongIdentifier id) const {
    auto it = m_typeConstructorRegistry.find(id);
    assert((it != m_typeConstructorRegistry.end()) && "TypeConstructor not registered in type system");
    return *std::get<0>(it->second);
}

babelwires::TypeConstructor*
babelwires::TypeSystem::addTypeConstructorInternal(LongIdentifier typeId, VersionNumber version,
                                                   std::unique_ptr<TypeConstructor> newTypeConstructor) {
    auto addResult = m_typeConstructorRegistry.emplace(std::pair<LongIdentifier, TypeConstructorInfo>{
        typeId, TypeConstructorInfo{std::move(newTypeConstructor), version}});
    assert(addResult.second && "TypeConstructor with that identifier already registered");
    return std::get<0>(addResult.first->second).get();
}

// TODO ALL VERY INEFFICIENT

void babelwires::TypeSystem::addRelatedTypes(LongIdentifier typeId, RelatedTypes relatedTypes) {
#ifndef NDEBUG
    const Type* const type = tryGetPrimitiveType(typeId);
    assert((type != nullptr) && "typeId is not the id of a registered type");
#endif
    std::sort(relatedTypes.m_subtypeIds.begin(), relatedTypes.m_subtypeIds.end());
    std::sort(relatedTypes.m_supertypeIds.begin(), relatedTypes.m_supertypeIds.end());
    for (const LongIdentifier& supertypeId : relatedTypes.m_supertypeIds) {
#ifndef NDEBUG
        const Type* const supertype = tryGetPrimitiveType(supertypeId);
        assert((supertype != nullptr) && "A supertype Id is not the id of a registered type");
        assert(type->verifySupertype(*supertype) && "A supertype is not suitable for the type");
#endif
        insertTypeId(m_relatedTypes[supertypeId].m_subtypeIds, typeId);
    }
    for (const LongIdentifier& subtypeId : relatedTypes.m_subtypeIds) {
#ifndef NDEBUG
        const Type* const subtype = tryGetPrimitiveType(subtypeId);
        assert((subtype != nullptr) && "A subtype ID is not the id of a registered type");
        assert(subtype->verifySupertype(*type) && "A supertype is not suitable for the type");
#endif
        insertTypeId(m_relatedTypes[subtypeId].m_supertypeIds, typeId);
    }
    assert((m_relatedTypes.find(typeId) == m_relatedTypes.end()) && "Related types already set for the given typeId");
    // TODO Cull tree?
    m_relatedTypes.insert({typeId, std::move(relatedTypes)});
}

const babelwires::TypeSystem::RelatedTypes& babelwires::TypeSystem::getRelatedTypes(const LongIdentifier& typeId) const {
    const auto it = m_relatedTypes.find(typeId);
    if (it != m_relatedTypes.end()) {
        return it->second;
    } else {
        return m_emptyRelatedTypes;
    }
}

babelwires::SubtypeOrder babelwires::TypeSystem::compareSubtype(const TypeRef& typeRefA,
                                                                          const TypeRef& typeRefB) const {
    if (typeRefA == typeRefB) {
        return SubtypeOrder::IsEquivalent;
    }
    return TypeRef::compareSubtypeHelper(*this, typeRefA, typeRefB);
}

bool babelwires::TypeSystem::isSubType(const TypeRef& typeRefA, const TypeRef& typeRefB) const {
    SubtypeOrder order = compareSubtype(typeRefA, typeRefB);
    return (order == SubtypeOrder::IsEquivalent) || (order == SubtypeOrder::IsSubtype);
}

bool babelwires::TypeSystem::isRelatedType(const TypeRef& typeRefA, const TypeRef& typeRefB) const {
    return compareSubtype(typeRefA, typeRefB) != SubtypeOrder::IsUnrelated;
}

bool babelwires::TypeSystem::isSubTypePrimitives(const LongIdentifier& typeIdA, const LongIdentifier& typeIdB) const {
    if (typeIdA == typeIdB) {
        return true;
    }
    for (auto parentId : getRelatedTypes(typeIdA).m_supertypeIds) {
        if (isSubTypePrimitives(parentId, typeIdB)) {
            return true;
        }
    }
    return false;
}

babelwires::SubtypeOrder
babelwires::TypeSystem::compareSubtypePrimitives(const LongIdentifier& typeIdA,
                                                         const LongIdentifier& typeIdB) const {
    if (typeIdA == typeIdB) {
        return SubtypeOrder::IsEquivalent;
    }
    if (isSubTypePrimitives(typeIdA, typeIdB)) {
        return SubtypeOrder::IsSubtype;
    }
    if (isSubTypePrimitives(typeIdB, typeIdA)) {
        return SubtypeOrder::IsSupertype;
    }
    return SubtypeOrder::IsUnrelated;
}


void babelwires::TypeSystem::addAllSubtypes(const LongIdentifier& typeId, TypeIdSet& subtypes) const {
    subtypes.emplace_back(typeId);
    for (auto subtypeId : getRelatedTypes(typeId).m_subtypeIds) {
        addAllSubtypes(subtypeId, subtypes);
    }
}

void babelwires::TypeSystem::addAllSupertypes(const LongIdentifier& typeId, TypeIdSet& supertypes) const {
    supertypes.emplace_back(typeId);
    for (auto subtypeId : getRelatedTypes(typeId).m_supertypeIds) {
        addAllSupertypes(subtypeId, supertypes);
    }
}

void babelwires::TypeSystem::addAllRelatedTypes(const LongIdentifier& typeId, TypeIdSet& typeIdSet) const {
    typeIdSet.emplace_back(typeId);
    addAllSubtypes(typeId, typeIdSet);
    addAllSupertypes(typeId, typeIdSet);
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getAllSubtypes(const LongIdentifier& typeId) const {
    TypeIdSet subtypes;
    addAllSubtypes(typeId, subtypes);
    removeDuplicates(subtypes);
    return subtypes;
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getAllSupertypes(const LongIdentifier& typeId) const {
    TypeIdSet supertypes;
    addAllSupertypes(typeId, supertypes);
    removeDuplicates(supertypes);
    return supertypes;
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getAllRelatedTypes(const LongIdentifier& typeId) const {
    TypeIdSet relatedTypes;
    addAllSubtypes(typeId, relatedTypes);
    addAllSupertypes(typeId, relatedTypes);
    removeDuplicates(relatedTypes);
    return relatedTypes;
}

void babelwires::TypeSystem::removeDuplicates(babelwires::TypeSystem::TypeIdSet& typeIds) {
    std::sort(typeIds.begin(), typeIds.end());
    typeIds.erase(std::unique(typeIds.begin(), typeIds.end()), typeIds.end());
}
