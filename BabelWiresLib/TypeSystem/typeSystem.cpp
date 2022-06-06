/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>

#include <BabelWiresLib/Enums/enum.hpp>

babelwires::UntypedTypeSystemRegistry::UntypedTypeSystemRegistry(std::string registryName) : UntypedRegistry(std::move(registryName)) {}

// TODO ALL VERY INEFFICIENT

void babelwires::UntypedTypeSystemRegistry::validateNewEntry(RegistryEntry* newEntry) const
{
    // Super-call.
    UntypedRegistry::validateNewEntry(newEntry);

    Type *const newType = static_cast<Type*>(newEntry);
    if (newType->getParentTypeId()) {
        const LongIdentifier parentTypeId = *newType->getParentTypeId();
        Type *const parentType = static_cast<Type*>(getEntryByIdentifierNonConst(parentTypeId));
        assert(parentType && "Parent type not known. Parent types must be registered before their children");
        newType->setParent(parentType);
        parentType->addChild(newType);
    }
}

babelwires::TypeSystem::TypeSystem()
    : Registry("Type system") {}

bool babelwires::TypeSystem::isSubType(LongIdentifier subtypeId, LongIdentifier supertypeId) const {
    const Type *const subtype = getEntryByIdentifier(subtypeId);
    assert(subtype && "The subtypeId was an unregistered type");
    const Type* current = subtype;
    while (current && (current->getIdentifier() != supertypeId)) {
        current = current->getParent();
    }
    return current;
}

bool babelwires::TypeSystem::isRelatedType(LongIdentifier typeAId, LongIdentifier typeBId) const {
    const Type *const typeA = getEntryByIdentifier(typeAId);
    assert(typeA && "The typeAId was an unregistered type");
    const Type *const typeB = getEntryByIdentifier(typeBId);
    assert(typeB && "The typeBId was an unregistered type");
    return typeA->isSubType(typeB) || typeB->isSubType(typeA);
}

namespace {
    void addAllSubtypesHelper(const babelwires::Type* type, babelwires::TypeSystem::TypeIdSet& subtypes) {
        for (const auto& child : type->getChildren()) {
            subtypes.emplace_back(child->getIdentifier());
            addAllSubtypesHelper(child, subtypes);
        }
    }

    void addAllSupertypesHelper(const babelwires::Type* type, babelwires::TypeSystem::TypeIdSet& supertypes) {
        const babelwires::Type *const parent = type->getParent();
        if (parent) {
            supertypes.emplace_back(parent->getIdentifier());
            addAllSupertypesHelper(parent, supertypes);
        }
    }
}

void babelwires::TypeSystem::addAllSubtypes(LongIdentifier typeId, TypeIdSet& subtypes) const {
    const Type *const type = getEntryByIdentifier(typeId);
    assert(type && "Encountered an unregistered type");
    subtypes.emplace_back(typeId);
    addAllSubtypesHelper(type, subtypes);
}

void babelwires::TypeSystem::addAllSupertypes(LongIdentifier typeId, TypeIdSet& supertypes) const {
    const Type *const type = getEntryByIdentifier(typeId);
    assert(type && "Encountered an unregistered type");
    supertypes.emplace_back(typeId);
    addAllSupertypesHelper(type, supertypes);
}

void babelwires::TypeSystem::addAllRelatedTypes(LongIdentifier typeId, TypeIdSet& relatedTypes) const {
    const Type *const type = getEntryByIdentifier(typeId);
    assert(type && "Encountered an unregistered type");
    relatedTypes.emplace_back(typeId);
    addAllSubtypesHelper(type, relatedTypes);
    addAllSupertypesHelper(type, relatedTypes);
}
