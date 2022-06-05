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

std::vector<babelwires::LongIdentifier> babelwires::TypeSystem::getAllSubtypes(LongIdentifier typeId) const {
    std::vector<babelwires::LongIdentifier> subtypes;
    subtypes.emplace_back(typeId);
    for (std::size_t index = 0; index < subtypes.size(); ++index)
    {
        const LongIdentifier currentId = subtypes[index];
        const Type *const current = getEntryByIdentifier(currentId);
        assert(current && "Encountered an unregistered type");
        const auto& children = current->getChildren();
        std::copy(children.begin(), children.end(), std::back_inserter(subtypes));
    }
    return subtypes;
}

std::vector<babelwires::LongIdentifier> babelwires::TypeSystem::getAllSupertypes(LongIdentifier typeId) const {
    std::vector<babelwires::LongIdentifier> supertypes;
    supertypes.emplace_back(typeId);
    LongIdentifier currentId = typeId;
    LongIdentifier parentId = typeId;
    do
    {
        currentId = parentId;
        const Type *const current = getEntryByIdentifier(currentId);
        assert(current && "Encountered an unregistered type");
        if (current->getParentTypeId()) {
            parentId = *current->getParentTypeId();
        }
    } while (parentId != currentId);
    return supertypes;
}
