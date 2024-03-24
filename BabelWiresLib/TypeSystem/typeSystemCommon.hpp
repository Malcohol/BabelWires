/**
 * Some common definitions used by type system classes.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    using PrimitiveTypeId = MediumId;
    using TypeConstructorId = MediumId;

    /// The maximum supported number of type arguments for a type constructor.
    static constexpr unsigned int c_maxNumTypeConstructorArguments = 2;

    /// The SubTypeOrder describes the relationship between two types.
    enum class SubtypeOrder {
        /// Both types have the same set of values.
        /// Note that this does not necessarily mean the types are equal.
        /// For example, two types could have the same range of values, but different defaults.
        IsEquivalent,
        /// Every value of the first type is a member of the second type.
        IsSubtype,
        /// Every value of the second type is a member of the first type.
        IsSupertype,
        /// The types are neither Subtypes nor Supertypes of each other.
        IsUnrelated
    };

    /// Swap IsSubtype and IsSupertype.
    inline SubtypeOrder reverseSubtypeOrder(SubtypeOrder order) {
        switch (order) {
            case SubtypeOrder::IsSubtype:
                return SubtypeOrder::IsSupertype;
            case SubtypeOrder::IsSupertype:
                return SubtypeOrder::IsSubtype;
            default:
                return order;
        }
    }

    /// The least order value greater than or equal to a or b, according to the partial order,
    /// where IsSubtype and IsSupertype are not comparable, and IsEquivalent is the least element.
    /// This allows two orders to be combined in a way that accounts for their compatibility.
    // TODO Needs a better name.
    inline SubtypeOrder subtypeOrderSupremum(SubtypeOrder a, SubtypeOrder b) {
        if (a == b) {
            return a;
        } else if (a == SubtypeOrder::IsEquivalent) {
            return b;
        } else if (b == SubtypeOrder::IsEquivalent) {
            return a;
        } else {
            return SubtypeOrder::IsUnrelated;
        }
    }
} // namespace babelwires
