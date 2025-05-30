/**
 * Some utility functions helpful for working with the SubtypeOrder.
 *
 * (C) 2025 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeSystemCommon.hpp>

namespace babelwires {

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

    /// If a is the subtypeOrder between T and U, and b is the subtypeOrder between V and W
    /// then this is the relationship between (T * U) and (V * W).
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
