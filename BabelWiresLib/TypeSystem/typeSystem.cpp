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

void babelwires::UntypedTypeSystemRegistry::validateNewEntry(const RegistryEntry* newEntry) const
{
    // Super-call.
    UntypedRegistry::validateNewEntry(newEntry);

    const Type *const newType = static_cast<const Type*>(newEntry);
    if (newType->getParentTypeId()) {
        const LongIdentifier parentTypeId = *newType->getParentTypeId();
        Type *const parentType = static_cast<Type*>(getEntryByIdentifierNonConst(parentTypeId));
        assert(parentType && "Parent type not known. Parent types must be registered before their children");
        newType->verifyParent(*parentType);
        parentType->addChild(parentTypeId);
    }
}

babelwires::TypeSystem::TypeSystem()
    : Registry("Type system") {}

bool babelwires::TypeSystem::isSubType(LongIdentifier subtypeId, LongIdentifier supertypeId) const {
    LongIdentifier currentId = subtypeId;
    while (currentId != supertypeId) {
        const Type *const current = getEntryByIdentifier(currentId);
        assert(current && "Encountered an unregistered type");
        if (!current->getParentTypeId()) {
            return false;
        }
        currentId = *current->getParentTypeId();
    }
    return true;
}

void babelwires::TypeSystem::addAllSubtypes(LongIdentifier typeId, TypeIdSet& subtypes) const {
    subtypes.emplace_back(typeId);
    const Type *const current = getEntryByIdentifier(typeId);
    assert(current && "Encountered an unregistered type");
    for (auto& childId : current->getChildren()) {
        addAllSubtypes(childId, subtypes);
    }
}

void babelwires::TypeSystem::addAllSupertypes(LongIdentifier typeId, TypeIdSet& supertypes) const {
    supertypes.emplace_back(typeId);
    const Type *const current = getEntryByIdentifier(typeId);
    assert(current && "Encountered an unregistered type");
    if (current->getParentTypeId()) {
        addAllSubtypes(*current->getParentTypeId(), supertypes);
    }
}
