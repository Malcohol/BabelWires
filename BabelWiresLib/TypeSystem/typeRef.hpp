/**
 * A TypeRef describes a type.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/TypeSystem/typePtr.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Identifiers/identifierRegistry.hpp>
#include <Common/Serialization/serializable.hpp>

#include <variant>
#include <vector>

namespace babelwires {
    class Type;
    class TypeRef;
    class TypeSystem;

    /// Holds the arguments to a TypeConstructor.
    class TypeConstructorArguments {
      public:
        TypeConstructorArguments() = default;
        TypeConstructorArguments(std::vector<TypeRef> typeArguments);
        TypeConstructorArguments(std::vector<TypeRef> typeArguments, std::vector<ValueHolder> valueArguments);
        ~TypeConstructorArguments();

        const std::vector<TypeRef>& getTypeArguments() const { return m_typeArguments; }
        std::vector<TypeRef>& getTypeArguments() { return m_typeArguments; }
        const std::vector<ValueHolder>& getValueArguments() const { return m_valueArguments; }
        std::vector<ValueHolder>& getValueArguments() { return m_valueArguments; }

        friend bool operator==(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return equals(a, b);
        }
        friend bool operator!=(const TypeConstructorArguments& a, const TypeConstructorArguments& b) {
            return !equals(a, b);
        }

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;

      private:
        /// The friend operators need to call an out-of-line implementation to avoid an include cycle.
        static bool equals(const TypeConstructorArguments& a, const TypeConstructorArguments& b);

      private:
        std::vector<TypeRef> m_typeArguments;
        std::vector<ValueHolder> m_valueArguments;
    };

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
        TypeRef(TypeConstructorId typeConstructorId, ValueHolder value0);
        TypeRef(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1);
        TypeRef(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1,
                ValueHolder value2);

        /// Attempt to find the type in the TypeSystem that this TypeRef describes.
        /// Returns null if this TypeRef does not resolve.
        TypePtr tryResolve(const TypeSystem& typeSystem) const;

        /// Find the type in the TypeSystem that this TypeRef describes.
        /// Throws an exception if this TypeRef does not resolve.
        TypePtr resolve(const TypeSystem& typeSystem) const;

        /// Find the type in the TypeSystem that this TypeRef describes.
        /// Asserts that this TypeRef resolves.
        TypePtr assertResolve(const TypeSystem& typeSystem) const;

        template<typename TYPE>
        TypePtrT<TYPE> resolveAs(const TypeSystem& typeSystem) const {
          return typeAs<TYPE>(resolve(typeSystem));
        }

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
        /// Warning: Be very careful with this, because you can assume very little about how a constructor uses its
        /// arguments. For example, the presence of a typeref argument does not guarantee that an instance of the
        /// corresponding type is contained in the result.
        template <typename Visitor, typename R = decltype(Visitor::operator()(std::monostate()))>
        R visit(Visitor& visitor) const;

        /// Binary exploration of two typeRefs. The typeRefs are expected to be structurally similar, or else the
        /// no argument operator of the visitor will be called.
        template <typename Visitor, typename R = decltype(Visitor::operator()(std::monostate(), std::monostate()))>
        static R visit(Visitor& visitor, const TypeRef& a, const TypeRef& b);

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

    template <> struct hash<babelwires::TypeConstructorArguments> {
        inline std::size_t operator()(const babelwires::TypeConstructorArguments& arguments) const {
            return arguments.getHash();
        }
    };
} // namespace std

#include <BabelWiresLib/TypeSystem/typeRef_inl.hpp>