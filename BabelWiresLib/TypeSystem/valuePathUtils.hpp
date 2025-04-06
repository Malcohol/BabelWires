/**
 * Functions for exploring values with Paths.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <optional>

namespace babelwires {
    class Path;
    class TypeSystem;

    /// Obtain a reference to a ValueHolder to data in start.
    std::optional<std::tuple<const Type&, const ValueHolder&>> tryFollow(const TypeSystem& typeSystem, const Type& type, const Path& path, const ValueHolder& start);

    /// Obtain a reference to a ValueHolder to data in a copy of start.
    /// Throws if the path cannot be followed.
    std::tuple<const Type&, ValueHolder&> followNonConst(const TypeSystem& typeSystem, const Type& type, const Path& path, ValueHolder& start);
}
