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
    class TypeRef : public ProjectVisitable, public Serializable {
      public:
        SERIALIZABLE(TypeRef, "type", void, 1);

        TypeRef();

        /// A TypeRef describing a primitive type.
        TypeRef(RegisteredTypeId typeId);

        /// A TypeRef describing a complex type, constructed by applying the TypeConstructor
        /// to the arguments.
        TypeRef(TypeConstructorId typeConstructorId, TypeConstructorArguments arguments);

        // Convenience constructors
        TypeRef(TypeConstructorId typeConstructorId, TypeRef typeRef0);
        TypeRef(TypeConstructorId typeConstructorId, TypeRef typeRef0, TypeRef typeRef1);
        TypeRef(TypeConstructorId typeConstructorId, EditableValueHolder value0);
        TypeRef(TypeConstructorId typeConstructorId, EditableValueHolder value0, EditableValueHolder value1);
        TypeRef(TypeConstructorId typeConstructorId, EditableValueHolder value0, EditableValueHolder value1, EditableValueHolder value2);

        /// Attempt to find the type in the TypeSystem that this TypeRef describes.
        /// Returns null if this TypeRef does not resolve.
        const Type* tryResolve(const TypeSystem& typeSystem) const;

        /// Find the type in the TypeSystem that this TypeRef describes.
        /// Throws an exception if this TypeRef does not resolve.
        const Type& resolve(const TypeSystem& typeSystem) const;

        /// Find the type in the TypeSystem that this TypeRef describes.
        /// Asserts that this TypeRef resolves.
        const Type& assertResolve(const TypeSystem& typeSystem) const;

        /// Return a human-readable version of the TypeRef.
        std::string toString() const;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        friend bool operator==(const TypeRef& a, const TypeRef& b) { return a.m_storage == b.m_storage; }
        friend bool operator!=(const TypeRef& a, const TypeRef& b) { return a.m_storage != b.m_storage; }

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;

        /// Does the TypeRef contain some data (other than the trivial std::monostate default state)?
        operator bool() const;

      private:
        /// Avoids locking the IdentifierRegistry multiple times.
        std::string toStringHelper(babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const;

        /// Returns a parsed type and a position just beyond that type.
        static std::tuple<babelwires::TypeRef, std::string_view::size_type> parseHelper(std::string_view str);

      public:
        /// Visit each of the cases of the TypeRef.
        template <typename Visitor, typename R = decltype(Visitor::operator()(std::monostate()))>
        R visit(Visitor& visitor) const;

      private:
        // TODO More compact storage.
        using ConstructedTypeData = std::tuple<TypeConstructorId, TypeConstructorArguments>;
        using Storage = std::variant<std::monostate, RegisteredTypeId, ConstructedTypeData>;

      private:
        Storage m_storage;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::TypeRef> {
        inline std::size_t operator()(const babelwires::TypeRef& typeRef) const { return typeRef.getHash(); }
    };
} // namespace std

#include <BabelWiresLib/TypeSystem/typeRef_inl.hpp>