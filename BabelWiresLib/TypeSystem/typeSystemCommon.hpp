/**
 * The SubTypeOrder describes the relationship between two types.
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

    static constexpr unsigned int s_maxNumTypeConstructorArguments = 2;

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
