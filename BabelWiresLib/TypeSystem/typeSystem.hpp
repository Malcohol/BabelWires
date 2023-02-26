/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/exceptions.hpp>

namespace babelwires {
    class TypeRef;

    class TypeSystemException : public ExceptionWithStream<TypeSystemException> {};

    class TypeSystem {
      public:
        virtual ~TypeSystem();

        template <typename TYPE, typename... ARGS,
                  std::enable_if_t<std::is_base_of_v<Type, TYPE>, std::nullptr_t> = nullptr>
        TYPE* addEntry(ARGS&&... args) {
            Type* newType = addPrimitiveType(TYPE::getThisIdentifier(), TYPE::getVersion(), std::make_unique<TYPE>(std::forward<ARGS>(args)...));
            return &newType->is<TYPE>();
        }

        template <typename TYPE, std::enable_if_t<std::is_base_of_v<Type, TYPE>, std::nullptr_t> = nullptr>
        const TYPE& getEntryByType() const {
            return getPrimitiveType(TYPE::getThisIdentifier()).template is<TYPE>();
        }

        const Type* tryGetPrimitiveType(LongIdentifier id) const;
        const Type& getPrimitiveType(LongIdentifier id) const;

        template <typename TYPE_CONSTRUCTOR, typename... ARGS,
                  std::enable_if_t<std::is_base_of_v<TypeConstructor, TYPE_CONSTRUCTOR>, std::nullptr_t> = nullptr>
        TYPE_CONSTRUCTOR* addTypeConstructor(ARGS&&... args) {
            TypeConstructor* newType = addTypeConstructorInternal(TYPE_CONSTRUCTOR::getThisIdentifier(), TYPE_CONSTRUCTOR::getVersion(), std::make_unique<TYPE_CONSTRUCTOR>(std::forward<ARGS>(args)...));
            return &newType->is<TYPE_CONSTRUCTOR>();
        }

        template <typename TYPE_CONSTRUCTOR, std::enable_if_t<std::is_base_of_v<TypeConstructor, TYPE_CONSTRUCTOR>, std::nullptr_t> = nullptr>
        const TYPE_CONSTRUCTOR& getTypeConstructorByType() const {
            return getTypeConstructor(TYPE_CONSTRUCTOR::getThisIdentifier()).template is<TYPE_CONSTRUCTOR>();
        }

        const TypeConstructor* tryGetTypeConstructor(LongIdentifier id) const;
        const TypeConstructor& getTypeConstructor(LongIdentifier id) const;

        using TypeIdSet = std::vector<TypeRef>;

        struct RelatedTypes {
            TypeIdSet m_supertypeIds;
            TypeIdSet m_subtypeIds;
        };

        /// All types must be already registered.
        /// Subtyping is managed seperately from the types themselves because a type may not know all its relations at
        /// construction time.
        void addRelatedTypes(LongIdentifier typeId, RelatedTypes relatedTypes);

        /// Confirm whether subtype is in fact a subtype of supertype (equality is allowed).
        bool isSubType(const TypeRef& subtypeId, const TypeRef& supertypeId) const;

        /// Confirm whether typeA is a subtype or supertype of type B (equality is allowed).
        bool isRelatedType(const TypeRef& typeAId, const TypeRef& typeBId) const;

        /// Return all the subtypes of type, including type.
        TypeIdSet getAllSubtypes(const TypeRef& typeId) const;

        /// Return all the supertypes, including type.
        TypeIdSet getAllSupertypes(const TypeRef& typeId) const;

        /// Return all subtypes and supertypes, including type.
        TypeIdSet getAllRelatedTypes(const TypeRef& typeId) const;

        /// Add typeId and all its subtypes to the set. Does not remove duplicates.
        void addAllSubtypes(const TypeRef& typeId, TypeIdSet& typeIdSet) const;

        /// Add typeId and all its supertypes to the set. Does not remove duplicates.
        void addAllSupertypes(const TypeRef& typeId, TypeIdSet& typeIdSet) const;

        /// Add typeId and all its subtypes and super types to the set. Does not remove duplicates.
        void addAllRelatedTypes(const TypeRef& typeId, TypeIdSet& typeIdSet) const;

        /// Convenience function.
        static void removeDuplicates(TypeIdSet& typeIds);

      protected:
        Type* addPrimitiveType(LongIdentifier typeId, VersionNumber version, std::unique_ptr<Type> newType);
        TypeConstructor* addTypeConstructorInternal(LongIdentifier typeConstructorId, VersionNumber version, std::unique_ptr<TypeConstructor> newTypeConstructor);

        const RelatedTypes& getRelatedTypes(const TypeRef& typeId) const;

      protected:
        using PrimitiveTypeInfo = std::tuple<std::unique_ptr<Type>, VersionNumber>;
        std::unordered_map<LongIdentifier, PrimitiveTypeInfo> m_primitiveTypeRegistry;

        using TypeConstructorInfo = std::tuple<std::unique_ptr<TypeConstructor>, VersionNumber>;
        std::unordered_map<LongIdentifier, TypeConstructorInfo> m_typeConstructorRegistry;

        std::unordered_map<TypeRef, RelatedTypes> m_relatedTypes;

        /// Used for types which have no relations.
        const RelatedTypes m_emptyRelatedTypes;
    };

} // namespace babelwires
