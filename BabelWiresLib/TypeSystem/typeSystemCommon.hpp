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
} // namespace babelwires
