/**
 * Functions for exploring values with Paths.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <optional>

namespace babelwires {
    class Path;
    class TypeSystem;

    /// Obtain a reference to a ValueHolder to data in start.
    /// Returns nullopt if the path cannot be followed.
    BABELWIRESLIB_API std::optional<std::tuple<const Type&, const ValueHolder&>> tryFollowPath(const TypeSystem& typeSystem, const Type& type, const Path& path, const ValueHolder& start);

    /// Obtain a reference to a ValueHolder to data in a copy of start.
    /// Asserts if the path cannot be followed.
    BABELWIRESLIB_API std::tuple<const Type&, ValueHolder&> assertFollowPathNonConst(const TypeSystem& typeSystem, const Type& type, const Path& path, ValueHolder& start);
    
    /// For each path, call the visitor at the corresponding value.
    /// Asserts if the paths cannot be followed.
    BABELWIRESLIB_API void assertVisitPathsNonConst(const TypeSystem& typeSystem, const Type& type, ValueHolder& value, const std::vector<Path>& paths, const std::function<void(const Type&, ValueHolder&)>& visitor);
}
