/**
 * TODO
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {

    // Sketch
    // TODO Could a built-in be provided by a plugin? How would we support this?
    enum class KnownType {
        // The built-in types
        Int,

        // All other types are assumed to be enums.
        Enum
    };

    ///
    LongIdentifier getBuiltInTypeId(KnownType type);

    /// Assumes any non-built in type is an enum.
    KnownType getTypeFromIdentifier(LongIdentifier id);
} // namespace babelwires
