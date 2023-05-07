/**
 * A MapFeature describes a mapping between types (ints or enums).
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>

namespace babelwires {
    /// A MapFeature describes a mapping between types.
    /// All the entries in the MapValue of a MapFeature are expected to be valid.
    class MapFeature2 : public SimpleValueFeature {
      public:
        MapFeature2(const TypeRef& sourceType, const TypeRef& targetType,
                    MapEntryData::Kind defaultFallbackKind = MapEntryData::Kind::All21);

        const MapValue& get() const;

      protected:
        /// The TypeSystem is available now, so the parent's TypeRef can be set.
        void doSetToDefault() override;

      private:
        /// TODO: This is disappointing. Storing these is a workaround for the fact that the Map's TypeRef cannot be
        /// created until a TypeSystem is available which only becomes possible in doSetToDefault.
        /// The need for the TypeSystem is because the typeRef uses an enum value for the fallback kind, which
        /// requires the EnumType to be looked-up. I could use an int instead, but the problem will just recur in
        /// future, so I will leave this here as a reminder to find a general approach.
        const TypeRef& m_sourceType;
        const TypeRef& m_targetType;
        MapEntryData::Kind m_defaultFallbackKind;
    };
} // namespace babelwires
