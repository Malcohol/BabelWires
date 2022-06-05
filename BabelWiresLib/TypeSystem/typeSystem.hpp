/**
 * TODO
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

        /// Return all the subtypes of type, including type.
        void addAllSubtypes(LongIdentifier typeId, TypeIdSet& subtypes) const;

        /// Return all the supertypes, including type.
        void addAllSupertypes(LongIdentifier typeId, TypeIdSet& supertypes) const;

        // TODO Move to Type

        using TypeSet = std::vector<const Type*>;
        
        /// Confirm whether subtype is in fact a subtype of supertype (equality is allowed).
        bool isSubType(const Type* subtype, const Type* supertype) const;

        /// Return all the subtypes of type, including type.
        void addAllSubtypes(const Type* type, TypeSet& subtypes) const;

        /// Return all the supertypes, including type.
        void addAllSupertypes(const Type* type, TypeSet& supertypes) const;
    };

} // namespace babelwires
