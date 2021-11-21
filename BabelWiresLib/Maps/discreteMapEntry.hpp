/**
 * DiscreteMapEntries define a relationship between a single source and a single target value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/mapEntry.hpp>

#include <Common/Utilities/hash.hpp>

namespace babelwires {
    /// A map entry with a single source value of discrete type.
    template <typename SOURCE_TYPE, typename TARGET_TYPE> struct DiscreteMapEntry {
        SOURCE_TYPE m_sourceValue;
        TARGET_TYPE m_targetValue;
        std::size_t getHash() const override;
        bool operator==(const MapEntry& other) const override;
    };
} // namespace babelwires

#include <BabelWiresLib/Maps/discreteMapEntry_inl.hpp>
