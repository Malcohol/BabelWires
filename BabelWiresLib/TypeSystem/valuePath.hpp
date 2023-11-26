/**
 * Functions for exploring values with FeaturePaths.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class FeaturePath;
    class TypeSystem;

    /// Obtain a reference to a ValueHolder to data in a copy of start.
    /// Throws if the path cannot be followed.
    std::tuple<const Type&, ValueHolder&> followNonConst(const TypeSystem& typeSystem, const Type& type, const FeaturePath& path, ValueHolder& start);
}
