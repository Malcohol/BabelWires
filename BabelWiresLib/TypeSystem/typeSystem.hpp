/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>

#include <Common/Registry/registry.hpp>
#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class TypeRef;

    /// The common C++ type of PrimitiveTypes.
    /// The type itself is carried by the subclass PrimitiveTypeEntryImpl.
    class PrimitiveTypeEntry : public RegistryEntry {
      public:
        PrimitiveTypeEntry(LongIdentifier identifier, VersionNumber version);

        virtual Type& getType() = 0;
        virtual const Type& getType() const = 0;
    };

    /// Implementation class of PrimitiveTypeEntry which contains an instance of the type.
    template <typename TYPE> class PrimitiveTypeEntryConcrete : public PrimitiveTypeEntry {
      public:
        /// Construct an entry and the type it describes together.
        /// If the type requires arguments (other than the TypeRef) those will be passed in by this constructor.
        template <typename... ARGS>
        PrimitiveTypeEntryConcrete(ARGS&&... args)
            : PrimitiveTypeEntry(TYPE::getThisIdentifier(), TYPE::getVersion())
            , m_type(std::forward<ARGS>(args)...) {}

        Type& getType() override { return m_type; }
        const Type& getType() const override { return m_type; }

      private:
        TYPE m_type;
    };

    class TypeSystem {
      public:
        TypeSystem();

        template<typename TYPE, typename... ARGS, std::enable_if_t<std::is_base_of_v<Type, TYPE>, std::nullptr_t> = nullptr>
        TYPE* addEntry(ARGS&&... args) {
          auto* entry = m_primitiveTypeRegistry.addEntry<PrimitiveTypeEntryConcrete<TYPE>>(std::forward<ARGS>(args)...);
          Type& type = entry->getType();
          return &type.is<TYPE>();
        }

        template<typename TYPE, std::enable_if_t<std::is_base_of_v<Type, TYPE>, std::nullptr_t> = nullptr>
        const TYPE& getEntryByType() const {
          const PrimitiveTypeEntry& entry = m_primitiveTypeRegistry.getRegisteredEntry(TYPE::getThisIdentifier());
          return entry.getType().is<TYPE>();
        }

        const Type* getEntryByIdentifier(LongIdentifier id) const;

        using TypeIdSet = std::vector<LongIdentifier>;

        struct RelatedTypes {
          TypeIdSet m_supertypeIds;
          TypeIdSet m_subtypeIds;
        };

        /// All types must be already registered.
        /// Subtyping is managed seperately from the types themselves because a type may not know all its relations at construction time.
        void addRelatedTypes(LongIdentifier typeId, RelatedTypes relatedTypes);

        /// Confirm whether subtype is in fact a subtype of supertype (equality is allowed).
        bool isSubType(const TypeRef& subtypeId, const TypeRef& supertypeId) const;

        /// Confirm whether typeA is a subtype or supertype of type B (equality is allowed).
        bool isRelatedType(const TypeRef& typeAId, const TypeRef& typeBId) const;

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
        Registry<PrimitiveTypeEntry> m_primitiveTypeRegistry;

        std::unordered_map<LongIdentifier, RelatedTypes> m_relatedTypes;

        /// Used for types which have no relations.
        const RelatedTypes m_emptyRelatedTypes;
    };

} // namespace babelwires
