/**
 * A TypeExp describes a type.
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
    class TypeExp;
    class TypeSystem;

    /// Holds the arguments to a TypeConstructor.
    class TypeConstructorArguments {
      public:
        TypeConstructorArguments() = default;
        TypeConstructorArguments(std::vector<TypeExp> typeArguments);
        TypeConstructorArguments(std::vector<TypeExp> typeArguments, std::vector<ValueHolder> valueArguments);
        ~TypeConstructorArguments();

        const std::vector<TypeExp>& getTypeArguments() const { return m_typeArguments; }
        std::vector<TypeExp>& getTypeArguments() { return m_typeArguments; }
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
        std::vector<TypeExp> m_typeArguments;
        std::vector<ValueHolder> m_valueArguments;
    };

    /// A TypeExp is an expression that describes a type. 
    /// It either stores an Id for registered types, or stores the id of a constructor and its arguments.
    class TypeExp : public ProjectVisitable, public Serializable {
      public:
        SERIALIZABLE(TypeExp, "type", void, 1);

        TypeExp();

        /// A TypeExp describing a primitive type.
        TypeExp(RegisteredTypeId typeId);

        /// A TypeExp describing a complex type, constructed by applying the TypeConstructor
        /// to the arguments.
        TypeExp(TypeConstructorId typeConstructorId, TypeConstructorArguments arguments);

        // Convenience constructors
        TypeExp(TypeConstructorId typeConstructorId, TypeExp typeRef0);
        TypeExp(TypeConstructorId typeConstructorId, TypeExp typeRef0, TypeExp typeRef1);
        TypeExp(TypeConstructorId typeConstructorId, ValueHolder value0);
        TypeExp(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1);
        TypeExp(TypeConstructorId typeConstructorId, ValueHolder value0, ValueHolder value1,
                ValueHolder value2);

        /// Attempt to find the type in the TypeSystem that this TypeExp describes.
        /// Returns null if this TypeExp does not resolve.
        TypePtr tryResolve(const TypeSystem& typeSystem) const;

        /// Find the type in the TypeSystem that this TypeExp describes.
        /// Throws an exception if this TypeExp does not resolve.
        TypePtr resolve(const TypeSystem& typeSystem) const;

        /// Find the type in the TypeSystem that this TypeExp describes.
        /// Asserts that this TypeExp resolves.
        TypePtr assertResolve(const TypeSystem& typeSystem) const;

        template<typename TYPE>
        TypePtrT<TYPE> resolveAs(const TypeSystem& typeSystem) const {
          return typeAs<TYPE>(resolve(typeSystem));
        }

        /// Return a human-readable version of the TypeExp.
        std::string toString() const;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        friend bool operator==(const TypeExp& a, const TypeExp& b) { return a.m_storage == b.m_storage; }
        friend bool operator!=(const TypeExp& a, const TypeExp& b) { return a.m_storage != b.m_storage; }

        /// Get a hash which can be used with std::hash.
        std::size_t getHash() const;

        /// Does the TypeExp contain some data (other than the trivial std::monostate default state)?
        operator bool() const;

      private:
        /// Avoids locking the IdentifierRegistry multiple times.
        std::string toStringHelper(babelwires::IdentifierRegistry::ReadAccess& identifierRegistry) const;

        /// Returns a parsed type and a position just beyond that type.
        static std::tuple<babelwires::TypeExp, std::string_view::size_type> parseHelper(std::string_view str);

      public:
        /// Visit each of the cases of the TypeExp.
        /// Warning: Be very careful with this, because you can assume very little about how a constructor uses its
        /// arguments. For example, the presence of a typeexp argument does not guarantee that an instance of the
        /// corresponding type is contained in the result.
        template <typename Visitor, typename R = decltype(Visitor::operator()(std::monostate()))>
        R visit(Visitor& visitor) const;

        /// Binary exploration of two typeRefs. The typeRefs are expected to be structurally similar, or else the
        /// no argument operator of the visitor will be called.
        template <typename Visitor, typename R = decltype(Visitor::operator()(std::monostate(), std::monostate()))>
        static R visit(Visitor& visitor, const TypeExp& a, const TypeExp& b);

      private:
        // TODO More compact storage.
        using ConstructedTypeData = std::tuple<TypeConstructorId, TypeConstructorArguments>;
        using Storage = std::variant<std::monostate, RegisteredTypeId, ConstructedTypeData>;

      private:
        Storage m_storage;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::TypeExp> {
        inline std::size_t operator()(const babelwires::TypeExp& typeExp) const { return typeExp.getHash(); }
    };

    template <> struct hash<babelwires::TypeConstructorArguments> {
        inline std::size_t operator()(const babelwires::TypeConstructorArguments& arguments) const {
            return arguments.getHash();
        }
    };
} // namespace std

#include <BabelWiresLib/TypeSystem/typeExp_inl.hpp>