/**
 * An Enum defines a set of options.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/TypeSystem/type.hpp"

#include <vector>

namespace babelwires {

    /// Defines the set of values available for enum features.
    /// See convenience class EnumWithCppEnum for a Enum which carries a corresponding C++ enum.
    class Enum : public Type {
      public:
        /// The set of values. We use Identifiers to get versionable serialization support.
        /// Note: This matches the result of REGISTERED_ID_VECTOR.
        using EnumValues = std::vector<Identifier>;

        /// Enums can be registered in a registry, so they need their own identifier and version.
        /// They also need a set of values and a way of identifying the default.
        /// The values object can be the "output" of the REGISTERED_ID_VECTOR macro.
        /// If parentTypeId is provided, then the parent type must itself be an enum, and must have
        /// values which include all the values of this enum. The values in that case are not required
        /// to be registered identifiers, since they will be resolved against the values in the parent.
        /// If parentTypeId is not provided, then the values must all be registered identifiers.
        Enum(LongIdentifier identifier, VersionNumber version, EnumValues values,
             unsigned int indexOfDefaultValue, std::optional<LongIdentifier> parentTypeId = {});

        /// Get the set of available enum values.
        const EnumValues& getEnumValues() const;

        /// Get the index of the default value in the vector of enum values.
        unsigned int getIndexOfDefaultValue() const;

        /// Get the index within EnumValues of the given id.
        unsigned int getIndexFromIdentifier(Identifier id) const;

        /// Get the identifier within EnumValues at the given index.
        Identifier getIdentifierFromIndex(unsigned int index) const;

        /// Is the identifier one of the values in the enum?
        /// This can resolve the discriminator in the identifier.
        bool isAValue(const Identifier& id) const;

        std::unique_ptr<Value> createValue() const override;

      protected:
        virtual bool verifyParent(const Type& parentType) const;

      private:
        EnumValues m_values;
        unsigned int m_indexOfDefaultValue;
    };
} // namespace babelwires
