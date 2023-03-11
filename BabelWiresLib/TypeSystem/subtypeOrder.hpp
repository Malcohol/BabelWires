/**
 * The SubTypeOrder describes the relationship between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

namespace babelwires {
    enum class SubtypeOrder { IsSubtype, IsSupertype, IsEquivalent, IsUnrelated };

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
}
