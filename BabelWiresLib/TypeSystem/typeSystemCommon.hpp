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
        /// Note that this does not necessarily mean the types are identical.
        /// For example, two types could have the same set of values, but different defaults.
        IsEquivalent,
        /// Every value of the first type is a member of the second type.
        IsSubtype,
        /// Every value of the second type is a member of the first type.
        IsSupertype,
        /// The types have some values in common.
        IsIntersecting,
        /// The types have no values in common.
        IsDisjoint
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

    /// TODO Provide an intuition for what this does.
    inline SubtypeOrder subtypeProduct(SubtypeOrder a, SubtypeOrder b) {
        if (a == b) {
            return a;
        } else if (a == SubtypeOrder::IsEquivalent) {
            return b;
        } else if (b == SubtypeOrder::IsEquivalent) {
            return a;
        } else if ((a == SubtypeOrder::IsDisjoint) || (b == SubtypeOrder::IsDisjoint)) {
            return SubtypeOrder::IsDisjoint;
        } else {
            return SubtypeOrder::IsIntersecting;
        }
    }

    /// Determine a SubtypeOrder value by comparing two ranges.
    template <typename T> SubtypeOrder subtypeFromRanges(const Range<T>& rangeA, const Range<T>& rangeB) {
        if (rangeA.m_min == rangeB.m_min) {
            if (rangeA.m_max == rangeB.m_max) {
                return SubtypeOrder::IsEquivalent;
            } else if (rangeA.m_max < rangeB.m_max) {
                return SubtypeOrder::IsSubtype;
            } else {
                return SubtypeOrder::IsSupertype;
            }
        } else if (rangeA.m_min < rangeB.m_min) {
            if (rangeA.m_max >= rangeB.m_max) {
                return SubtypeOrder::IsSupertype;
            } else if (rangeA.m_max >= rangeB.m_min) {
                return SubtypeOrder::IsIntersecting;
            } else {
                return SubtypeOrder::IsDisjoint;
            }
        } else {
            if (rangeA.m_max <= rangeB.m_max) {
                return SubtypeOrder::IsSubtype;
            } else if (rangeA.m_min <= rangeB.m_max) {
                return SubtypeOrder::IsIntersecting;
            } else {
                return SubtypeOrder::IsDisjoint;
            }
        }
    }
} // namespace babelwires
