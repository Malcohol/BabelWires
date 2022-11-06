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

    class Type : public RegistryEntry {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Type);
        Type(LongIdentifier identifier, VersionNumber version, std::optional<LongIdentifier> parentTypeId = {});
        
        virtual std::unique_ptr<Value> createValue() const = 0;

        /// Confirm that the supertype when resolved in the type system is the expected parent.
        /// The default implementation asserts.
        virtual bool verifySupertype(const Type& supertype) const;
    };
}
