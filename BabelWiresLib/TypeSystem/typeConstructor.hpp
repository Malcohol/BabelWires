/**
 * A TypeConstructor constructs a type from other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <shared_mutex>

namespace babelwires {

    /// A TypeConstructor constructs a type from other types.
    /// The types when constructed are stored in a mutable cache, which is locked by a mutex.
    class TypeConstructor {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(TypeConstructor);

        /// Get the constructed type from the cache, or construct a new one.
        const Type* getOrConstructType(const TypeSystem& typeSystem, const TypeConstructorArguments& arguments) const;

        /// This is supplied by the TYPE_CONSTRUCTOR macro.
        virtual LongIdentifier getTypeConstructorId() const = 0;

        /// TypeConstructors are expected to have fixed arity.
        virtual unsigned int getArity() const = 0;

        virtual std::unique_ptr<Type> constructType(TypeRef newTypeRef,
                                                    const std::vector<const Type*>& arguments) const = 0;

        // TODO Support for deduced subtyping, something like isSubtype and getSubtypes.

      private:
        /// A mutex which ensures thread-safe access to the cache.
        mutable std::shared_mutex m_mutexForCache;

        /// A cache which stops the system ending up with multiple copies of the same constructed type.
        mutable std::unordered_map<TypeConstructorArguments, std::unique_ptr<Type>> m_cache;
    };

    /// A convenience type which can used by type constructors for the type they want to construct
    /// where the class for the type doesn't implement getTypeRef.
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

/// Type constructors need to be registered.
/// For now, assume this is always done statically.
/// The NAME of a type constructor is a format string with positional arguments for all its type arguments.
/// For example, the NAME of the binary product type constructor might be something like "({0} * {1})".
/// Regular brackets can be written with "{{" and "}}". Non-positional arguments "{}" are not supported.
#define TYPE_CONSTRUCTOR(IDENTIFIER, NAME, UUID, VERSION)                                                              \
    static babelwires::LongIdentifier getThisIdentifier() { return REGISTERED_LONGID(IDENTIFIER, NAME, UUID); }        \
    static babelwires::VersionNumber getVersion() { return VERSION; }                                                  \
    babelwires::LongIdentifier getTypeConstructorId() const override { return getThisIdentifier(); }

/// Intended mainly for testing.
#define TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(IDENTIFIER, VERSION)                                                       \
    static babelwires::LongIdentifier getThisIdentifier() { return IDENTIFIER; }                                       \
    static babelwires::VersionNumber getVersion() { return VERSION; }                                                  \
    babelwires::LongIdentifier getTypeConstructorId() const override { return getThisIdentifier(); }
