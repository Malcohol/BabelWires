/**
 * MapEntryData define a relationship between source and target values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Serialization/serializable.hpp>
#include <Common/Utilities/result.hpp>

#include <Common/types.hpp>

namespace babelwires {
    class TypeSystem;
    class TypeRef;
    class Type;

    class MapEntryData : public Serializable, public Cloneable, public ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(MapEntryData);
        CLONEABLE_ABSTRACT(MapEntryData);
        SERIALIZABLE_ABSTRACT(MapEntryData, void);

        virtual ~MapEntryData();
        virtual std::size_t getHash() const = 0;
        virtual bool operator==(const MapEntryData& other) const = 0;
        bool operator!= (const MapEntryData& other) const { return !(*this == other); }
        
        Result validate(const TypeSystem& typeSystem, const TypeRef& sourceTypeRef, const TypeRef& targetTypeRef, bool isLastEntry) const;

        /// MapEntryData subclasses have a fixed hierarchy.
        enum class Kind {
            OneToOne,
            AllToOne,
            AllToSame,

            NUM_KINDS
        };

        virtual Kind getKind() const = 0;

        static std::string getKindName(Kind kind);
        static bool isFallback(Kind kind);

        /// Create a MapEntryData of the given kind.
        static std::unique_ptr<MapEntryData> create(const TypeSystem& typeSystem, const TypeRef& sourceTypeRef, const TypeRef& targetTypeRef, Kind kind);

      protected:
        virtual Result doValidate(const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType) const = 0;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::MapEntryData> {
        inline std::size_t operator()(const babelwires::MapEntryData& entry) const { return entry.getHash(); }
    };
} // namespace std
