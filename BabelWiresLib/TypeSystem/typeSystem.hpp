/**
 * The TypeSystem manages the set of types and the relationship between them.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/TypeSystem/typePtr.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructor.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemException.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class TypeExp;

    class TypeSystem {
      public:
        TypeSystem();
        virtual ~TypeSystem();

        template <typename TYPE, typename... ARGS,
                  std::enable_if_t<std::is_base_of_v<Type, TYPE>, std::nullptr_t> = nullptr>
        const TYPE* addEntry(ARGS&&... args) {
            const Type* newType = addRegisteredType(TYPE::getThisIdentifier(), TYPE::getVersion(), makeType<TYPE>(std::forward<ARGS>(args)...));
            return &newType->is<TYPE>();
        }

        template <typename TYPE, std::enable_if_t<std::is_base_of_v<Type, TYPE>, std::nullptr_t> = nullptr>
        TypePtrT<TYPE> getEntryByType() const {
            return typeAs<TYPE>(getRegisteredType(TYPE::getThisIdentifier()));
        }

        TypePtr tryGetRegisteredType(RegisteredTypeId id) const;
        TypePtr getRegisteredType(RegisteredTypeId id) const;

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

        using TypeIdSet = std::vector<RegisteredTypeId>;

        /// Determine how typeA and typeB are related by the subtype order.
        SubtypeOrder compareSubtype(const TypeExp& typeExpA, const TypeExp& typeExpB) const;

        /// Confirm whether typeExpA is a subtype of typeExpB (equality is allowed).
        bool isSubType(const TypeExp& typeExpA, const TypeExp& typeExpB) const;

        /// Do the two types have some values in common?
        bool isRelatedType(const TypeExp& typeExpA, const TypeExp& typeExpB) const;

        TypeIdSet getAllRegisteredTypes() const;

        /// Get all the registered types tagged with the given tag.
        TypeIdSet getTaggedRegisteredTypes(Type::Tag tag) const;

      protected:
        const Type* addRegisteredType(LongId typeId, VersionNumber version, TypePtr newType);
        TypeConstructor* addTypeConstructorInternal(TypeConstructorId typeConstructorId, VersionNumber version, std::unique_ptr<TypeConstructor> newTypeConstructor);

      protected:
        using RegisteredTypeInfo = std::tuple<TypePtr, VersionNumber>;
        std::unordered_map<RegisteredTypeId, RegisteredTypeInfo> m_registeredTypeRegistry;

        using TypeConstructorInfo = std::tuple<std::unique_ptr<TypeConstructor>, VersionNumber>;
        std::unordered_map<TypeConstructorId, TypeConstructorInfo> m_typeConstructorRegistry;

        /// Fast look-up of tagged types.
        std::unordered_map<Type::Tag, std::vector<RegisteredTypeId>> m_taggedRegisteredTypes;
    };

} // namespace babelwires
