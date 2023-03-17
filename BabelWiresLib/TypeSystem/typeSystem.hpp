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
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class TypeRef;

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

        const Type* tryGetPrimitiveType(PrimitiveTypeId id) const;
        const Type& getPrimitiveType(PrimitiveTypeId id) const;

        template <typename TYPE_CONSTRUCTOR, typename... ARGS,
                  std::enable_if_t<std::is_base_of_v<TypeConstructor<TYPE_CONSTRUCTOR::c_arity>, TYPE_CONSTRUCTOR>, std::nullptr_t> = nullptr>
        TYPE_CONSTRUCTOR* addTypeConstructor(ARGS&&... args) {
            TypeConstructor<TYPE_CONSTRUCTOR::c_arity>* newType = addTypeConstructorInternal<TYPE_CONSTRUCTOR::c_arity>(TYPE_CONSTRUCTOR::getThisIdentifier(), TYPE_CONSTRUCTOR::getVersion(), std::make_unique<TYPE_CONSTRUCTOR>(std::forward<ARGS>(args)...));
            return &newType->template is<TYPE_CONSTRUCTOR>();
        }

        // TODO Should be able to pick up N from TYPE_CONSTRUCTOR
        template <unsigned int N, typename TYPE_CONSTRUCTOR, std::enable_if_t<std::is_base_of_v<TypeConstructor<N>, TYPE_CONSTRUCTOR>, std::nullptr_t> = nullptr>
        const TYPE_CONSTRUCTOR& getTypeConstructorByType() const {
            return getTypeConstructor<N>(TYPE_CONSTRUCTOR::getThisIdentifier()).template is<TYPE_CONSTRUCTOR>();
        }

        template<unsigned int N>
        const TypeConstructor<N>* tryGetTypeConstructor(TypeConstructorId id) const;

        template<unsigned int N>
        const TypeConstructor<N>& getTypeConstructor(TypeConstructorId id) const;

        using TypeIdSet = std::vector<PrimitiveTypeId>;

        struct RelatedTypes {
            TypeIdSet m_supertypeIds;
            TypeIdSet m_subtypeIds;
        };

        /// All types must be already registered.
        /// Subtyping is managed seperately from the types themselves because a type may not know all its relations at
        /// construction time.
        void addRelatedTypes(PrimitiveTypeId typeId, RelatedTypes relatedTypes);

        /// Determine whether typeA and typeB are related by the subtype order.
        SubtypeOrder compareSubtype(const TypeRef& typeRefA, const TypeRef& typeRefB) const;

        /// Confirm whether typeRefA is in fact a subtype of typeRefB (equality is allowed).
        bool isSubType(const TypeRef& typeRefA, const TypeRef& typeRefB) const;

        /// Confirm whether typeA is a subtype or supertype of type B (equality is allowed).
        bool isRelatedType(const TypeRef& typeRefA, const TypeRef& typeRefB) const;

        /// Return all the subtypes of type, including type.
        TypeIdSet getAllSubtypes(const PrimitiveTypeId& typeId) const;

        /// Return all the supertypes, including type.
        TypeIdSet getAllSupertypes(const PrimitiveTypeId& typeId) const;

        /// Return all subtypes and supertypes, including type.
        TypeIdSet getAllRelatedTypes(const PrimitiveTypeId& typeId) const;

        /// Add typeId and all its subtypes to the set. Does not remove duplicates.
        void addAllSubtypes(const PrimitiveTypeId& typeId, TypeIdSet& typeIdSet) const;

        /// Add typeId and all its supertypes to the set. Does not remove duplicates.
        void addAllSupertypes(const PrimitiveTypeId& typeId, TypeIdSet& typeIdSet) const;

        /// Add typeId and all its subtypes and super types to the set. Does not remove duplicates.
        void addAllRelatedTypes(const PrimitiveTypeId& typeId, TypeIdSet& typeIdSet) const;

        /// Convenience function.
        static void removeDuplicates(TypeIdSet& typeIds);

        /// Determine whether primitive typeA and primitive typeB are related by the subtype order.
        SubtypeOrder compareSubtypePrimitives(const PrimitiveTypeId& typeIdA, const PrimitiveTypeId& typeIdB) const;

      protected:
        Type* addPrimitiveType(LongId typeId, VersionNumber version, std::unique_ptr<Type> newType);

        template<unsigned int N>
        TypeConstructor<N>* addTypeConstructorInternal(TypeConstructorId typeConstructorId, VersionNumber version, std::unique_ptr<TypeConstructor<N>> newTypeConstructor);

        const RelatedTypes& getRelatedTypes(const PrimitiveTypeId& typeId) const;

        bool isSubTypePrimitives(const PrimitiveTypeId& typeIdA, const PrimitiveTypeId& typeIdB) const;

      protected:
        using PrimitiveTypeInfo = std::tuple<std::unique_ptr<Type>, VersionNumber>;
        std::unordered_map<PrimitiveTypeId, PrimitiveTypeInfo> m_primitiveTypeRegistry;

        template<unsigned int N>
        using TypeConstructorInfo = std::tuple<std::unique_ptr<TypeConstructor<N>>, VersionNumber>;

        template<unsigned int N>
        using TypeConstructorRegistry = std::unordered_map<TypeConstructorId, TypeConstructorInfo<N>>;

        static_assert(c_maxNumTypeConstructorArguments == 2);
        std::tuple<TypeConstructorRegistry<1>, TypeConstructorRegistry<2>> m_typeConstructorRegistry;

        std::unordered_map<PrimitiveTypeId, RelatedTypes> m_relatedTypes;

        /// Used for types which have no relations.
        const RelatedTypes m_emptyRelatedTypes;
    };

} // namespace babelwires

#include <BabelWiresLib/TypeSystem/typeSystem_inl.hpp>