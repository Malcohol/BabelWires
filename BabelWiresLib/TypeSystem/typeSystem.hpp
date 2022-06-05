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
        void validateNewEntry(const RegistryEntry* newEntry) const override;
    };

    class TypeSystem : public Registry<Type, UntypedTypeSystemRegistry> {
      public:
        TypeSystem();

        /// Confirm whether subtype is in fact a subtype of supertype (equality is allowed).
        bool isSubType(LongIdentifier subtypeId, LongIdentifier supertypeId) const;

        /// Return all the subtypes of type, including type.
        std::vector<LongIdentifier> getAllSubtypes(LongIdentifier typeId) const;

        /// Return all the supertypes, including type.
        std::vector<LongIdentifier> getAllSupertypes(LongIdentifier typeId) const;
    };

} // namespace babelwires
