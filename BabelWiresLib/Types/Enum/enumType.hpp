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
    /// A type offering a choice from a set of named values.
    /// The user should care only about the selected value, and not about the particular index, which is
    /// an implementation detail and could change in future.
    /// The particular advantage of an EnumFeature is that values can be reordered and new values can be added
    /// without requiring versioning code to adapt old serialized data.
    /// If the index is meaningful to the user, consider using an integer with with ValueNames.
    class EnumType : public Type {
      public:
        /// The set of values. We use Identifiers to get versionable serialization support.
        /// The ValueSet vector may not contain duplicates.
        /// Note: This matches the result of REGISTERED_ID_VECTOR.
        using ValueSet = std::vector<ShortId>;

        /// Enums need a set of values and a way of identifying the default.
        /// The values object can be the "output" of the REGISTERED_ID_VECTOR macro.
        /// The values must all be registered identifiers.
        EnumType(ValueSet values, unsigned int indexOfDefaultValue);

        /// Get the set of available enum values.
        const ValueSet& getValueSet() const;

        /// Get the index of the default value in the vector of enum values.
        unsigned int getIndexOfDefaultValue() const;

        /// Get the index within ValueSet of the given id.
        unsigned int getIndexFromIdentifier(ShortId id) const;

        /// Return the index within ValueSet of the given id, or -1.
        int tryGetIndexFromIdentifier(ShortId id) const;

        /// Get the identifier within ValueSet at the given index.
        ShortId getIdentifierFromIndex(unsigned int index) const;

        /// Is the identifier one of the values in the enum?
        /// This can resolve the discriminator in the identifier.
        bool isAValue(const ShortId& id) const;

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool isValidValue(const TypeSystem& typeSystem, const Value& v) const override;

        std::string getKind() const override;

        virtual bool verifySupertype(const Type& supertype) const;

      private:
        /// The enum values in their intended order.
        ValueSet m_values;
        /// Supports faster lookup for identifier-based queries.
        std::unordered_map<ShortId, int> m_valueToIndex;
        unsigned int m_indexOfDefaultValue;
    };
} // namespace babelwires
