/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <Common/Identifiers/identifier.hpp>

#include <optional>

namespace babelwires {

    /// A type describes a valid set of values.
    /// Note that a value can be an instance of more than one type.
    /// A type is a subtypes of another type when all its values are valid
    /// values of the other.
    /// Information about subtype relationships is managed by the TypeSystem.
    class Type {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Type);

        /// Create a new Value representing a default instance of the type.
        virtual NewValueHolder createValue(const TypeSystem& typeSystem) const = 0;

        /// Is the value v an element of this type.
        virtual bool isValidValue(const Value& v) const = 0;

        /// Get a TypeRef that describes this type.
        /// Primitive types get an implementation of this method from the PRIMITIVE_TYPE macro.
        /// Complex types constructed by TypeConstructors must provide their own implementation.
        virtual TypeRef getTypeRef() const = 0;

        /// Return a short string which defines the kind of values this type handles.
        virtual std::string getKind() const = 0;

        /// An abstract type just acts as a placeholder in the type system and cannot
        /// be used in a number of situations.
        /// An example is AbstractPercussionSet in SeqWires, which is used as a common base
        /// type for all enums of percussion instruments, but is not intended to be an actual type.
        /// The default implementation returns false, so types need to opt-in to being abstract.
        virtual bool isAbstract() const;

        /// Confirm that the supertype is the expected parent.
        /// The default implementation asserts.
        virtual bool verifySupertype(const Type& supertype) const;

        /// Convenience function which returns a human-readable version of the type's TypeRef.
        std::string getName() const;
    };
} // namespace babelwires
