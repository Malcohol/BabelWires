/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include "Common/Registry/registry.hpp"
#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class Type;

    class UntypedTypeSystemRegistry : public UntypedRegistry 
    {
      public:
        UntypedTypeSystemRegistry(std::string registryName);
      protected:
        void validateNewEntry(RegistryEntry* newEntry) const override;
    };

    class TypeSystem : public Registry<Type, UntypedTypeSystemRegistry> {
      public:
        TypeSystem();

        using TypeIdSet = std::vector<LongIdentifier>;

        /// Confirm whether subtype is in fact a subtype of supertype (equality is allowed).
        bool isSubType(LongIdentifier subtypeId, LongIdentifier supertypeId) const;

        /// Confirm whether typeA is a subtype or supertype of type B (equality is allowed).
        bool isRelatedType(LongIdentifier typeAId, LongIdentifier typeBId) const;

        /// Return all the subtypes of type, including type.
        void addAllSubtypes(LongIdentifier typeId, TypeIdSet& subtypes) const;

        /// Return all the supertypes, including type.
        void addAllSupertypes(LongIdentifier typeId, TypeIdSet& supertypes) const;

        /// Return all subtypes and supertypes, including type.
        void addAllRelatedTypes(LongIdentifier typeId, TypeIdSet& relatedTypes) const;
    };

} // namespace babelwires
