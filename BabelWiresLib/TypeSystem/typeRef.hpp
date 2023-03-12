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
#include <BabelWiresLib/TypeSystem/subtypeOrder.hpp>

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
        using PrimitiveTypeId = LongIdentifier;
        using TypeConstructorId = LongIdentifier;

        TypeRef();

        /// A TypeRef describing a primitive type.
        TypeRef(PrimitiveTypeId typeId);

        /// A TypeRef describing a complex type, constructed by applying the TypeConstructor
        /// to the arguments.
        TypeRef(TypeConstructorId typeConstructorId, TypeConstructorArguments arguments);

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
        static SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem, const TypeRef& typeRefA, const TypeRef& typeRefB);

      private:
        /// Avoids locking the IdentifierRegistry multiple times.
        std::string toStringHelper(babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const;

        /// Returns a parsed type and a position just beyond that type.
        static std::tuple<babelwires::TypeRef, std::string_view::size_type> parseHelper(std::string_view str);

      private:
        using ConstructedTypeData = std::tuple<TypeConstructorId, TypeConstructorArguments>;
        using Storage = std::variant<std::monostate, PrimitiveTypeId, ConstructedTypeData>;

      private:
        Storage m_storage;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::TypeRef> {
        inline std::size_t operator()(const babelwires::TypeRef& typeRef) const { return typeRef.getHash(); }
    };
} // namespace std
