/**
 * A MapFeature describes a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/heavyValueFeature.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

namespace babelwires {
    /// A MapFeature describes a mapping between types.
    /// All the entries in the MapValue of a MapFeature are expected to be valid.
    ///
    /// StandardMapFeature (below) supports a single source and single target type.
    /// However, in general MapFeatures are permitted to have more than one allowed
    /// source and/or target type. In theory, this is unnecessary because exactly the
    /// same flexibility would be available by putting separate maps in the branches
    /// of a union. However, the MapEditor already has a type selector to support
    /// sub- and supertypes, so re-using it to choose between allowed types provides
    /// a nicer UI.
    ///
    /// Type theory note: Maps have a fallback behaviour and therefore their source
    /// types do not have to treated contravariantly. Instead, map values can be used if
    /// they have a "related" source type (i.e. in a subtype or supertype relationship).
    /// Maps with unrelated source types wouldn't be useful, so I judged it better to
    /// exclude them.
    /// Target types are treated in the usual covariant way.
    class MapFeature : public HeavyValueFeature<MapValue> {
      public:
        /// Get the default map value used by this feature.
        virtual MapValue getDefaultMapValue() const;

        struct AllowedTypes {
            std::vector<TypeRef> m_typeRefs;
            unsigned int m_indexOfDefault = 0;

            const TypeRef& getDefaultTypeRef() const { return m_typeRefs[m_indexOfDefault]; }
            bool isRelatedToSome(const TypeSystem& typeSystem, const TypeRef& type) const;
            bool isSubtypeOfSome(const TypeSystem& typeSystem, const TypeRef& type) const;
        };

        virtual void getAllowedSourceTypeRefs(AllowedTypes& allowedTypesOut) const = 0;
        virtual void getAllowedTargetTypeRefs(AllowedTypes& allowedTypesOut) const = 0;

      protected:
        std::string doGetValueType() const override;
        void onBeforeSetValue(const MapValue& newValue) const override;
        void doSetToDefault() override;

        /// Convenience method for subclasses that want to override getDefaultMapValue, letting them
        /// easily return a default with the appropriate kind of fallback.
        MapValue getStandardDefaultMapValue(MapEntryData::Kind fallbackKind) const;
    };

    /// A MapFeature which allows only a single source and target type.
    class StandardMapFeature : public MapFeature {
      public:
        StandardMapFeature(TypeRef sourceId, TypeRef targetId);

        void getAllowedSourceTypeRefs(AllowedTypes& allowedTypesOut) const override;
        void getAllowedTargetTypeRefs(AllowedTypes& allowedTypesOut) const override;

      private:
        TypeRef m_allowedSourceTypeRef;
        TypeRef m_allowedTargetTypeRef;
    };
} // namespace babelwires
