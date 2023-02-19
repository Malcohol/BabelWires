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

babelwires::PrimitiveTypeEntry::PrimitiveTypeEntry(LongIdentifier identifier, VersionNumber version)
    : RegistryEntry(identifier, version) {}

namespace {
    void insertTypeId(babelwires::TypeSystem::TypeIdSet& typeIds, babelwires::LongIdentifier typeId) {
        auto it = std::upper_bound(typeIds.begin(), typeIds.end(), typeId);
        typeIds.insert(it, typeId);
    }
} // namespace

babelwires::TypeSystem::TypeSystem()
    : m_primitiveTypeRegistry("Primitive Type Registry") {}

const babelwires::Type* babelwires::TypeSystem::tryGetPrimitiveType(LongIdentifier id) const {
    const auto* entry = m_primitiveTypeRegistry.getEntryByIdentifier(id);
    return entry ? &entry->getType() : nullptr;
}

const babelwires::Type& babelwires::TypeSystem::getPrimitiveType(LongIdentifier id) const {
    return m_primitiveTypeRegistry.getRegisteredEntry(id).getType();
}

// TODO ALL VERY INEFFICIENT

void babelwires::TypeSystem::addRelatedTypes(LongIdentifier typeId, RelatedTypes relatedTypes) {
#ifndef NDEBUG
    const Type* const type = tryGetPrimitiveType(typeId);
    assert((type != nullptr) && "typeId is not the id of a registered type");
#endif
    std::sort(relatedTypes.m_subtypeIds.begin(), relatedTypes.m_subtypeIds.end());
    std::sort(relatedTypes.m_supertypeIds.begin(), relatedTypes.m_supertypeIds.end());
    for (LongIdentifier supertypeId : relatedTypes.m_supertypeIds) {
#ifndef NDEBUG
        const Type* const supertype = tryGetPrimitiveType(supertypeId);
        assert((supertype != nullptr) && "A supertype Id is not the id of a registered type");
        assert(type->verifySupertype(*supertype) && "A supertype is not suitable for the type");
#endif
        insertTypeId(m_relatedTypes[supertypeId].m_subtypeIds, typeId);
    }
    for (LongIdentifier subtypeId : relatedTypes.m_subtypeIds) {
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

const babelwires::TypeSystem::RelatedTypes& babelwires::TypeSystem::getRelatedTypes(LongIdentifier typeId) const {
    const auto it = m_relatedTypes.find(typeId);
    if (it != m_relatedTypes.end()) {
        return it->second;
    } else {
        return m_emptyRelatedTypes;
    }
}

bool babelwires::TypeSystem::isSubType(const TypeRef& subtypeId, const TypeRef& supertypeId) const {
    /*
    if (subtypeId == supertypeId) {
        return true;
    }
    for (auto parentId : getRelatedTypes(subtypeId).m_supertypeIds) {
        if (isSubType(parentId, supertypeId)) {
            return true;
        }
    }
    */
    return false;
}

bool babelwires::TypeSystem::isRelatedType(const TypeRef& typeAId, const TypeRef& typeBId) const {
    // return isSubType(typeAId, typeBId) || isSubType(typeBId, typeAId);
    return false;
}

void babelwires::TypeSystem::addAllSubtypes(LongIdentifier typeId, TypeIdSet& subtypes) const {
    subtypes.emplace_back(typeId);
    for (auto subtypeId : getRelatedTypes(typeId).m_subtypeIds) {
        addAllSubtypes(subtypeId, subtypes);
    }
}

void babelwires::TypeSystem::addAllSupertypes(LongIdentifier typeId, TypeIdSet& supertypes) const {
    supertypes.emplace_back(typeId);
    for (auto subtypeId : getRelatedTypes(typeId).m_supertypeIds) {
        addAllSupertypes(subtypeId, supertypes);
    }
}

void babelwires::TypeSystem::addAllRelatedTypes(LongIdentifier typeId, TypeIdSet& typeIdSet) const {
    typeIdSet.emplace_back(typeId);
    addAllSubtypes(typeId, typeIdSet);
    addAllSupertypes(typeId, typeIdSet);
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getAllSubtypes(LongIdentifier typeId) const {
    TypeIdSet subtypes;
    addAllSubtypes(typeId, subtypes);
    removeDuplicates(subtypes);
    return subtypes;
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getAllSupertypes(LongIdentifier typeId) const {
    TypeIdSet supertypes;
    addAllSupertypes(typeId, supertypes);
    removeDuplicates(supertypes);
    return supertypes;
}

babelwires::TypeSystem::TypeIdSet babelwires::TypeSystem::getAllRelatedTypes(LongIdentifier typeId) const {
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
