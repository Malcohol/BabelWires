/**
 * A TypeConstructor constructs a type from other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <shared_mutex>

namespace babelwires {

    /// A TypeConstructor constructs a type from other types.
    /// A mutable cache ensures that each type is only constructed once.
    class TypeConstructor {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(TypeConstructor);

        /// Get the constructed type from the cache, or construct a new one.
        const Type* getOrConstructType(const TypeSystem& typeSystem, const TypeConstructorArguments& arguments) const;

        /// This is supplied by the TYPE_CONSTRUCTOR macro.
        virtual TypeConstructorId getTypeConstructorId() const = 0;

        /// Are two types constructed by this type constructor related by subtyping?
        /// By default, this returns IsUnrelated.
        virtual SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem,
                                                  const TypeConstructorArguments& argumentsA,
                                                  const TypeConstructorArguments& argumentsB) const;

        /// Is this a type constructed by this type related to the type other?
        /// By default, this returns IsUnrelated.
        /// Assumption: The subtype relation between two constructed types never depends on an axiom which involves both
        /// type constructors. (Things like distributivity laws would break this assumption, but I doubt they'd be
        /// needed.)
        virtual SubtypeOrder compareSubtypeHelper(const TypeSystem& typeSystem,
                                                  const TypeConstructorArguments& arguments,
                                                  const TypeRef& other) const;

      protected:
        /// Construct the new type.
        /// The newTypeRef is provided to allow implementations to move it into the constructed type.
        virtual std::unique_ptr<Type> constructType(const TypeSystem& typeSystem, TypeRef newTypeRef, const std::vector<const Type*>& typeArguments,
                                                    const std::vector<EditableValueHolder>& valueArguments) const = 0;

      private:
        /// A mutex which ensures thread-safe access to the cache.
        /// Use a shared-only lock on the assumption that the majority of simultaneous queries are not for the
        // same TypeRef.
        mutable std::shared_mutex m_mutexForCache;

        /// A cache which stops the system ending up with multiple copies of the same constructed type.
        mutable std::unordered_map<TypeConstructorArguments, std::unique_ptr<Type>> m_cache;
    };

    /// A convenience class which can used by type constructors for the type they want to construct.
    /// It provides an implementing getTypeRef.
    /// Type constructors are not obliged to use this template.
    template <typename T> class ConstructedType : public T {
      public:
        template <typename... ARGS>
        ConstructedType(TypeRef typeRef, ARGS&&... args)
            : T(std::forward<ARGS>(args)...)
            , m_typeRef(std::move(typeRef)) {}

        TypeRef getTypeRef() const override { return m_typeRef; }

      private:
        TypeRef m_typeRef;
    };
} // namespace babelwires

/// Intended mainly for testing.
#define TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(IDENTIFIER, VERSION)                                                       \
    static babelwires::TypeConstructorId getThisIdentifier() { return IDENTIFIER; }                                    \
    static babelwires::VersionNumber getVersion() { return VERSION; }                                                  \
    babelwires::TypeConstructorId getTypeConstructorId() const override { return getThisIdentifier(); }

/// Type constructors need to be registered.
/// For now, assume this is always done statically.
/// The NAME of a type constructor is a format string with positional arguments for all its type arguments.
/// For example, the NAME of the binary product type constructor might be something like "{0} * {1}".
/// Regular brackets can be written with "{{" and "}}". Non-positional arguments "{}" are not supported.
#define TYPE_CONSTRUCTOR(IDENTIFIER, NAME, UUID, VERSION)                                                              \
    TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(BW_MEDIUM_ID(IDENTIFIER, NAME, UUID), VERSION)
