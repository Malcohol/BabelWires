/**
 * MapEntryData define a relationship between source and target values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/projectVisitable.hpp"
#include "BabelWiresLib/TypeSystem/type.hpp"

#include "Common/Serialization/serializable.hpp"
#include "Common/Cloning/cloneable.hpp"

#include <Common/types.hpp>

namespace babelwires {
    class Type;

    class MapEntryData : public Serializable, public Cloneable, public ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(MapEntryData);
        CLONEABLE_ABSTRACT(MapEntryData);
        SERIALIZABLE_ABSTRACT(MapEntryData, void);

        virtual ~MapEntryData();
        virtual std::size_t getHash() const = 0;
        virtual bool operator==(const MapEntryData& other) const = 0;
        virtual bool isValid(const Type& sourceType, const Type& targetType) const = 0;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::MapEntryData> {
        inline std::size_t operator()(const babelwires::MapEntryData& entry) const { return entry.getHash(); }
    };
} // namespace std
