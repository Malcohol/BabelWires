/**
 * A TypeRef identifies a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Serialization/serializable.hpp>
#include <Common/Identifiers/identifier.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>

#include <variant>
#include <array>

namespace babelwires {
    class Type;
    class TypeSystem;

    class TypeRef : public ProjectVisitable {
      public:
        using PrimitiveTypeId = LongIdentifier;
        using TypeConstructorId = LongIdentifier;

        TypeRef();
        TypeRef(PrimitiveTypeId typeId);
        
        using Arguments = std::vector<TypeRef>;
        TypeRef(TypeConstructorId typeConstructorId, Arguments arguments);

        const Type* tryResolve(const TypeSystem& typeSystem) const;
        const Type& resolve(const TypeSystem& typeSystem) const;

        /// Return a human-readable version of the TypeRef.
        std::string toString() const;

        /// Return a serializable version of the TypeRef.
        std::string serializeToString() const;

        /// Parse a string as a TypeRef.
        static TypeRef deserializeFromString(std::string_view str);

        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        bool operator==(const TypeRef& other) const;
        bool operator!=(const TypeRef& other) const;
        
        /// Note: This orders TypeRefs using identifiers NOT SUBTYPING!
        bool operator<(const TypeRef& other) const;

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;

      private:
        /// Avoids locking the IdentifierRegistry multiple times.
        void toStringHelper(std::ostream& os, babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const;

        /// Returns a parsed type and a position just beyond that type.
        static std::tuple<babelwires::TypeRef, std::string_view::size_type> parseHelper(std::string_view str);

      private:
        // TODO Consider a hack where the first element of the vector is actually treated as a constructorId.
        using ConstructedTypeData = std::tuple<TypeConstructorId, Arguments>;
        using Storage = std::variant<std::nullptr_t, PrimitiveTypeId, ConstructedTypeData>;

      private:
        Storage m_typeDescription;
    };
} // namespace babelwires


namespace std {
    template <> struct hash<babelwires::TypeRef> {
        inline std::size_t operator()(const babelwires::TypeRef& typeRef) const { return typeRef.getHash(); }
    };
} // namespace std
