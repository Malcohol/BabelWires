/**
 * A EnumMap defines a mapping between two enum types.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Maps/map.hpp>
#include <BabelWiresLib/Maps/mapEntry.hpp>

#include <vector>

namespace babelwires {
    ///
    template <typename VALUE_TYPE> class EnumMap : public Map {
      public:
        using EnumMaplet = DiscreteMapEntry<unsigned int, VALUE_TYPE>;

        EnumMap();

        //std::unordered_map<unsigned int, unsigned int> createMapForApplication() const;
    };
} // namespace babelwires

#include <BabelWiresLib/Maps/enumMap_inl.hpp>