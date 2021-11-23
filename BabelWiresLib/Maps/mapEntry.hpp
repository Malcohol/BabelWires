/**
 * MapEntries define a relationship between values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/projectVisitable.hpp"

#include "Common/Serialization/serializable.hpp"
#include "Common/Cloning/cloneable.hpp"

#include <Common/types.hpp>

namespace babelwires {
    class MapEntry : public Serializable, public Cloneable, public ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(MapEntry);
        CLONEABLE_ABSTRACT(MapEntry);
        SERIALIZABLE_ABSTRACT(MapEntry, "mapEntry", void);

        virtual ~MapEntry() = default;
        virtual std::size_t getHash() const = 0;
        virtual bool operator==(const MapEntry& other) const = 0;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::MapEntry> {
        inline std::size_t operator()(const babelwires::MapEntry& entry) const { return entry.getHash(); }
    };
} // namespace std
