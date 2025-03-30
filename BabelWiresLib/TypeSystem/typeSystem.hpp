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
        TypeSystem();
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
                  std::enable_if_t<std::is_base_of_v<TypeConstructor, TYPE_CONSTRUCTOR>, std::nullptr_t> = nullptr>
        TYPE_CONSTRUCTOR* addTypeConstructor(ARGS&&... args) {
            TypeConstructor* newType = addTypeConstructorInternal(TYPE_CONSTRUCTOR::getThisIdentifier(), TYPE_CONSTRUCTOR::getVersion(), std::make_unique<TYPE_CONSTRUCTOR>(std::forward<ARGS>(args)...));
            return &newType->template is<TYPE_CONSTRUCTOR>();
        }

        template <typename TYPE_CONSTRUCTOR, std::enable_if_t<std::is_base_of_v<TypeConstructor, TYPE_CONSTRUCTOR>, std::nullptr_t> = nullptr>
        const TYPE_CONSTRUCTOR& getTypeConstructorByType() const {
            return getTypeConstructor(TYPE_CONSTRUCTOR::getThisIdentifier()).template is<TYPE_CONSTRUCTOR>();
        }

        const TypeConstructor* tryGetTypeConstructor(TypeConstructorId id) const;
        const TypeConstructor& getTypeConstructor(TypeConstructorId id) const;

        using TypeIdSet = std::vector<PrimitiveTypeId>;

        /// Determine whether typeA and typeB are related by the subtype order.
        SubtypeOrder compareSubtype(const TypeRef& typeRefA, const TypeRef& typeRefB) const;

        /// Confirm whether typeRefA is in fact a subtype of typeRefB (equality is allowed).
        bool isSubType(const TypeRef& typeRefA, const TypeRef& typeRefB) const;

        /// Confirm whether typeA is a subtype or supertype of type B (equality is allowed).
        bool isRelatedType(const TypeRef& typeRefA, const TypeRef& typeRefB) const;

        /// Determine whether primitive typeA and primitive typeB are related by the subtype order.
        SubtypeOrder compareSubtypePrimitives(const PrimitiveTypeId& typeIdA, const PrimitiveTypeId& typeIdB) const;

        TypeIdSet getAllPrimitiveTypes() const;

        /// Get all the primitive types tagged with the given tag.
        TypeIdSet getTaggedPrimitiveTypes(Type::Tag tag) const;

      protected:
        Type* addPrimitiveType(LongId typeId, VersionNumber version, std::unique_ptr<Type> newType);
        TypeConstructor* addTypeConstructorInternal(TypeConstructorId typeConstructorId, VersionNumber version, std::unique_ptr<TypeConstructor> newTypeConstructor);

      protected:
        using PrimitiveTypeInfo = std::tuple<std::unique_ptr<Type>, VersionNumber>;
        std::unordered_map<PrimitiveTypeId, PrimitiveTypeInfo> m_primitiveTypeRegistry;

        using TypeConstructorInfo = std::tuple<std::unique_ptr<TypeConstructor>, VersionNumber>;
        std::unordered_map<TypeConstructorId, TypeConstructorInfo> m_typeConstructorRegistry;

        /// Fast look-up of tagged types.
        std::unordered_map<Type::Tag, std::vector<PrimitiveTypeId>> m_taggedPrimitiveTypes;
    };

} // namespace babelwires
