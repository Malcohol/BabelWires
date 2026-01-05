/**
 * MapEntryData define a relationship between source and target values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/projectVisitable.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Enum/enumWithCppEnum.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Serialization/serializable.hpp>
#include <Common/Utilities/result.hpp>

#include <Common/types.hpp>

namespace babelwires {
    class TypeSystem;
    class TypeExp;
    class Type;

#define BW_MAP_ENTRY_FALLBACK_KIND(X)                                                                                  \
    X(One21, "OneToOne", "f498676a-fe06-4ee0-a841-5e563a6324f4")                                                       \
    X(All21, "AllToOne", "d25f684d-1dad-475c-bb07-f387ee61cc3c")                                                       \
    X(All2Sm, "AllToSame", "49293192-43b2-4902-820a-e11d519b152b")

    /// The enum that determines the algorithm used.
    class MapEntryFallbackKind : public babelwires::EnumType {
      public:
        REGISTERED_TYPE("FallbackKind", "MapEntryFallbackKind", "11e020d5-526e-412d-aa9f-ac464ea34d26", 1);
        MapEntryFallbackKind();

        ENUM_DEFINE_CPP_ENUM(BW_MAP_ENTRY_FALLBACK_KIND);
    };

    class MapEntryData : public Serializable, public Cloneable, public ProjectVisitable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(MapEntryData);
        CLONEABLE_ABSTRACT(MapEntryData);
        SERIALIZABLE_ABSTRACT(MapEntryData, void);

        virtual ~MapEntryData();
        virtual std::size_t getHash() const = 0;
        virtual bool operator==(const MapEntryData& other) const = 0;
        bool operator!=(const MapEntryData& other) const { return !(*this == other); }

        Result validate(const TypeSystem& typeSystem, const Type& sourceType, const Type& targetType,
                        bool isLastEntry) const;

        using Kind = MapEntryFallbackKind::Value;
        virtual Kind getKind() const = 0;

        static std::string getKindName(Kind kind);
        static bool isFallback(Kind kind);

        /// Create a MapEntryData of the given kind.
        static std::unique_ptr<MapEntryData> create(const TypeSystem& typeSystem, const Type& sourceTypeExp,
                                                    const Type& targetTypeExp, Kind kind);

        /// Return the sourceValue or nullptr if this entry doesn't have one.
        virtual const ValueHolder* tryGetSourceValue() const;
        /// Return the sourceValue or assert if this entry doesn't have one.
        const ValueHolder& getSourceValue() const;
        /// The default implementation asserts.
        virtual void setSourceValue(ValueHolder value);

        /// Return the targetValue or nullptr if this entry doesn't have one.
        virtual const ValueHolder* tryGetTargetValue() const;
        /// Return the targetValue or assert if this entry doesn't have one.
        const ValueHolder& getTargetValue() const;
        /// The default implementation asserts.
        virtual void setTargetValue(ValueHolder value);

      protected:
        virtual Result doValidate(const TypeSystem& typeSystem, const Type& sourceType,
                                  const Type& targetType) const = 0;
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::MapEntryData> {
        inline std::size_t operator()(const babelwires::MapEntryData& entry) const { return entry.getHash(); }
    };
} // namespace std
