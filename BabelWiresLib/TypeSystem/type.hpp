/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include <Common/Registry/registry.hpp>
#include <Common/Identifiers/identifier.hpp>

#include <optional>

namespace babelwires {

    /// A type describes a valid set of values.
    /// Note that information about subtype relationships is kept in the TypeSystem.
    class Type : public RegistryEntry {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Type);
        Type(LongIdentifier identifier, VersionNumber version);
        
        virtual std::unique_ptr<Value> createValue() const = 0;

        /// An abstract type just acts as a placeholder in the type system and will not
        /// be permitted in a number of situations.
        /// An example is AbstractPercussionType in seqwires, which is used as a common base
        /// type for all enums of percussion instruments, but is not intended to be an actual type.
        /// The default implementation returns false, so types need to opt-in to being abstract.
        virtual bool isAbstract() const;

        /// Confirm that the supertype when resolved in the type system is the expected parent.
        /// The default implementation asserts.
        virtual bool verifySupertype(const Type& supertype) const;
    };
}
