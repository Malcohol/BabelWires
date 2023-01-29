/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Registry/registry.hpp>
#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class Type;

    class TypeSystem : public Registry<Type> {
      public:
        TypeSystem();

        using TypeIdSet = std::vector<LongIdentifier>;

        struct RelatedTypes {
          TypeIdSet m_supertypeIds;
          TypeIdSet m_subtypeIds;
        };

        /// All types must be already registered.
        /// Subtyping is managed seperately from the types themselves because a type may not know all its relations at construction time.
        void addRelatedTypes(LongIdentifier typeId, RelatedTypes relatedTypes);

        /// Confirm whether subtype is in fact a subtype of supertype (equality is allowed).
        bool isSubType(LongIdentifier subtypeId, LongIdentifier supertypeId) const;

        /// Confirm whether typeA is a subtype or supertype of type B (equality is allowed).
        bool isRelatedType(LongIdentifier typeAId, LongIdentifier typeBId) const;

        /// Return all the subtypes of type, including type.
        TypeIdSet getAllSubtypes(LongIdentifier typeId) const;

        /// Return all the supertypes, including type.
        TypeIdSet getAllSupertypes(LongIdentifier typeId) const;

        /// Return all subtypes and supertypes, including type.
        TypeIdSet getAllRelatedTypes(LongIdentifier typeId) const;

        /// Add typeId and all its subtypes to the set. Does not remove duplicates.
        void addAllSubtypes(LongIdentifier typeId, TypeIdSet& typeIdSet) const;

        /// Add typeId and all its supertypes to the set. Does not remove duplicates.
        void addAllSupertypes(LongIdentifier typeId, TypeIdSet& typeIdSet) const;

        /// Add typeId and all its subtypes and super types to the set. Does not remove duplicates.
        void addAllRelatedTypes(LongIdentifier typeId, TypeIdSet& typeIdSet) const;

        /// Convenience function.
        static void removeDuplicates(TypeIdSet& typeIds);

      protected:
        const RelatedTypes& getRelatedTypes(LongIdentifier typeId) const;

      protected:
        std::unordered_map<LongIdentifier, RelatedTypes> m_relatedTypes;

        /// Used for types which have no relations.
        const RelatedTypes m_emptyRelatedTypes;
    };

} // namespace babelwires
