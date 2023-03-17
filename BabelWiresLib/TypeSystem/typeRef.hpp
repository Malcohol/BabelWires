/**
 * A TypeRef describes a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/TypeSystem/typeConstructorArguments.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/serializable.hpp>

#include <array>
#include <variant>

namespace babelwires {
    class Type;
    class TypeSystem;

    /// A TypeRef describes a type by storing an Id if it is a primitive type,
    /// or storing the id of its constructor and its arguments.
    class TypeRef : public ProjectVisitable {
      public:
        TypeRef();

        /// A TypeRef describing a primitive type.
        TypeRef(PrimitiveTypeId typeId);

        /// A TypeRef describing a complex type, constructed by applying the TypeConstructor
        /// to the arguments.
        TypeRef(TypeConstructorId typeConstructorId, TypeRef argument)
            : m_storage(ConstructedStorage<1>{std::make_shared<ConstructedTypeData<1>>(ConstructedTypeData<1>{typeConstructorId, {std::move(argument)}})}) {}

        TypeRef(TypeConstructorId typeConstructorId, TypeRef argument0, TypeRef argument1)
            : m_storage(ConstructedStorage<2>{std::make_shared<ConstructedTypeData<2>>(ConstructedTypeData<2>{typeConstructorId, {std::move(argument0), std::move(argument1)}})}) {}

        template<unsigned int N>
        explicit TypeRef(TypeConstructorId typeConstructorId, TypeConstructorArguments<N> arguments)
            : m_storage(ConstructedStorage<N>{std::make_shared<ConstructedTypeData<N>>(ConstructedTypeData<N>{typeConstructorId, std::move(arguments)})}) {}

        /// Attempt to find the type in the TypeSystem that this TypeRef describes.
        const Type* tryResolve(const TypeSystem& typeSystem) const;

        /// Find the type in the TypeSystem that this TypeRef describes.
        const Type& resolve(const TypeSystem& typeSystem) const;

        /// Return a human-readable version of the TypeRef.
        std::string toString() const;

        /// Return a serializable version of the TypeRef.
        std::string serializeToString() const;

        /// Parse a string as a TypeRef.
        static TypeRef deserializeFromString(std::string_view str);

        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        friend bool operator==(const TypeRef& a, const TypeRef& b) { return a.m_storage == b.m_storage; }
        friend bool operator!=(const TypeRef& a, const TypeRef& b) { return a.m_storage != b.m_storage; }

        /// Note: This orders TypeRefs using identifiers NOT SUBTYPING!
        friend bool operator<(const TypeRef& a, const TypeRef& b) { return a.m_storage < b.m_storage; }

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;

        /// Used by the TypeSystem to compare the type typeRefs using the subtype relationship.
        static SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeRef& typeRefA,
                                                 const TypeRef& typeRefB);

      private:
        /// Avoids locking the IdentifierRegistry multiple times.
        std::string toStringHelper(babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const;

        /// Returns a parsed type and a position just beyond that type.
        static std::tuple<babelwires::TypeRef, std::string_view::size_type> parseHelper(std::string_view str);

      private:
        struct CompareSubtypeVisitor;

      private:
        template <unsigned int N> struct ConstructedTypeData {
            ConstructedTypeData(TypeConstructorId, TypeConstructorArguments<N>&& args);
            TypeConstructorId m_typeConstructorId;
            TypeConstructorArguments<N> m_arguments;

            friend bool operator==(const ConstructedTypeData& a, const ConstructedTypeData& b) {
                return (a.m_typeConstructorId == b.m_typeConstructorId) && (a.m_arguments == b.m_arguments);
            }
            friend bool operator!=(const ConstructedTypeData& a, const ConstructedTypeData& b) { return !(a == b); }
            friend bool operator<(const ConstructedTypeData& a, const ConstructedTypeData& b) { return a.m_arguments < b.m_arguments; }

        };

        template <unsigned int N> struct ConstructedStorage {
            ConstructedTypeData<N>* operator->() const {
              return m_ptr.get();
            }

            friend bool operator==(const ConstructedStorage& a, const ConstructedStorage& b) {
                return (a.m_ptr == b.m_ptr) || (*a.m_ptr == *b.m_ptr);
            }
            friend bool operator!=(const ConstructedStorage& a, const ConstructedStorage& b) { return !(a == b); }
            friend bool operator<(const ConstructedStorage& a, const ConstructedStorage& b) 
            {
              return (a.m_ptr == b.m_ptr) || (*a.m_ptr < *b.m_ptr);
            }

            std::shared_ptr<ConstructedTypeData<N>> m_ptr;
        };

        using Storage = std::variant<std::monostate, PrimitiveTypeId, ConstructedStorage<1>, ConstructedStorage<2>>;

      private:
        Storage m_storage;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::TypeRef> {
        inline std::size_t operator()(const babelwires::TypeRef& typeRef) const { return typeRef.getHash(); }
    };
} // namespace std
