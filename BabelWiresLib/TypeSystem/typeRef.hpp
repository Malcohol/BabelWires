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

#include <variant>
#include <array>

namespace babelwires {

    class TypeRef : public ProjectVisitable {
      public:
        using PrimitiveTypeId = LongIdentifier;
        using TypeConstructorId = LongIdentifier;

        TypeRef();
        TypeRef(PrimitiveTypeId typeId);
        TypeRef(TypeConstructorId unaryTypeConstructorId, TypeRef argument);
        TypeRef(const TypeRef& other);

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

      private:
        template<int N> using Arguments = std::array<TypeRef, N>;
        template<int N> using HigherOrderTypeData = std::tuple<TypeConstructorId, std::unique_ptr<Arguments<N>>>;

        // TODO Include all cases up to a fixed N.
        using Storage = std::variant<std::nullptr_t, PrimitiveTypeId, HigherOrderTypeData<1>>;

        static Storage deepCopy(const Storage& otherStorage);

      private:
        Storage m_typeDescription;
    };
} // namespace babelwires