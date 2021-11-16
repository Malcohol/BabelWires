/**
 * A Map defines a how values between two types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <vector>

#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class Map {
      public:

        bool operator==(const Map& other) const;
        bool operator!=(const Map& other) const;

        LongIdentifier getSourceType() const;
        LongIdentifier getTargetType() const;

        std::size_t getHash() const;
    };
}