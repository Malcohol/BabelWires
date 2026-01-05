/**
 * A TypeConstructor constructs a type from other types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typePtr.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>

#include <Common/Identifiers/identifier.hpp>
#include <Common/Identifiers/registeredIdentifier.hpp>

#include <shared_mutex>
#include <variant>

namespace babelwires {

    /// A TypeConstructor constructs a type from other types.
    /// A mutable cache ensures that each type is only constructed once.
    class TypeConstructor {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(TypeConstructor);

        virtual ~TypeConstructor();

        /// Get the constructed type from the cache, or construct a new one.
        /// Returns null if the type cannot be constructed.
        TypePtr tryGetOrConstructType(const TypeSystem& typeSystem,
                                          const TypeConstructorArguments& arguments) const;

        /// Get the constructed type from the cache, or construct a new one.
        /// Throws a TypeSystemException if the type cannot be constructed.
        TypePtr getOrConstructType(const TypeSystem& typeSystem, const TypeConstructorArguments& arguments) const;

        /// This is supplied by the TYPE_CONSTRUCTOR macro.
        virtual TypeConstructorId getTypeConstructorId() const = 0;

      protected:
        /// Construct the new type, return an existing type (if the constructor is just a pure wrapper)
        /// or throw a TypeSystemException if it cannot be constructed.
        /// The newTypeExp is provided to allow implementations to move it into a newly constructed type.
        /// Resolved types corresponding to the type arguments are provided. However, newly constructed
        /// types should be passed TypeExps from the arguments rather than using the TypeExps of the 
        /// resolved types.
        virtual TypePtr constructType(const TypeSystem& typeSystem, TypeExp newTypeExp,
                                                    const TypeConstructorArguments& arguments,
                                                    const std::vector<TypePtr>& resolvedTypeArguments) const = 0;

      private:
        using TypeOrError = std::variant<TypePtr, std::string>;
        
        TypeOrError getOrConstructTypeInternal(const TypeSystem& typeSystem,
                                                         const TypeConstructorArguments& arguments) const;

      private:
        /// A mutex which ensures thread-safe access to the cache.
        /// Use a shared-only lock on the assumption that the majority of simultaneous queries are not for the
        // same TypeExp.
        mutable std::shared_mutex m_mutexForCache;

        using PerTypeStorage = std::variant<WeakTypePtr, std::string>;

        /// A cache which stops the system ending up with multiple copies of the same constructed type.
        mutable std::unordered_map<TypeConstructorArguments, PerTypeStorage> m_cache;
    };

    /// A convenience class which can used by type constructors for the type they want to construct.
    /// It provides an implementation of getTypeExp.
    /// Type constructors are not obliged to use this template.
    template <typename T> class ConstructedType : public T {
      public:
        template <typename... ARGS>
        ConstructedType(TypeExp typeExp, ARGS&&... args)
            : T(std::forward<ARGS>(args)...)
            , m_typeExp(std::move(typeExp)) {}

        TypeExp getTypeExp() const override { return m_typeExp; }

      private:
        TypeExp m_typeExp;
    };
} // namespace babelwires

/// Intended mainly for testing.
#define TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(IDENTIFIER, VERSION)                                                       \
    static babelwires::TypeConstructorId getThisIdentifier() {                                                         \
        return IDENTIFIER;                                                                                             \
    }                                                                                                                  \
    static babelwires::VersionNumber getVersion() {                                                                    \
        return VERSION;                                                                                                \
    }                                                                                                                  \
    babelwires::TypeConstructorId getTypeConstructorId() const override {                                              \
        return getThisIdentifier();                                                                                    \
    }

/// Type constructors need to be registered.
/// For now, assume this is always done statically.
/// The NAME of a TypeConstructor is treated as a format string. See the comments in typeNameFormatter.
#define TYPE_CONSTRUCTOR(IDENTIFIER, NAME, UUID, VERSION)                                                              \
    TYPE_CONSTRUCTOR_WITH_REGISTERED_ID(BW_MEDIUM_ID(IDENTIFIER, NAME, UUID), VERSION)
