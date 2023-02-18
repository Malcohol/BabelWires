/**
 * An Enum defines a set of options.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>

#include <vector>
#include <unordered_map>

namespace babelwires {

    /// Defines the set of values available for enum features.
    /// See convenience class EnumWithCppEnum for a Enum which carries a corresponding C++ enum.
    class Enum : public Type {
      public:
        /// The set of values. We use Identifiers to get versionable serialization support.
        /// The EnumValues vector may not contain duplicates.
        /// Note: This matches the result of REGISTERED_ID_VECTOR.
        using EnumValues = std::vector<Identifier>;

        /// Enums need a set of values and a way of identifying the default.
        /// The values object can be the "output" of the REGISTERED_ID_VECTOR macro.
        /// The values must all be registered identifiers.
        Enum(EnumValues values, unsigned int indexOfDefaultValue);

        /// Get the set of available enum values.
        const EnumValues& getEnumValues() const;

        /// Get the index of the default value in the vector of enum values.
        unsigned int getIndexOfDefaultValue() const;

        /// Get the index within EnumValues of the given id.
        unsigned int getIndexFromIdentifier(Identifier id) const;

        /// Return the index within EnumValues of the given id, or -1.
        int tryGetIndexFromIdentifier(Identifier id) const;

        /// Get the identifier within EnumValues at the given index.
        Identifier getIdentifierFromIndex(unsigned int index) const;

        /// Is the identifier one of the values in the enum?
        /// This can resolve the discriminator in the identifier.
        bool isAValue(const Identifier& id) const;

        std::unique_ptr<Value> createValue() const override;

        virtual bool verifySupertype(const Type& supertype) const;

      private:
        /// The enum values in their intended order.
        EnumValues m_values;
        /// Supports faster lookup for identifier-based queries.
        std::unordered_map<Identifier, int> m_valueToIndex;
        unsigned int m_indexOfDefaultValue;
    };
} // namespace babelwires
