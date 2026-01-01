/**
 * MapType is the type for MapValue.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

namespace babelwires {

    /// Type theory note: Maps have a fallback behaviour and therefore their source
    /// types do not have to treated contravariantly: In theory, a map with any source
    /// type can be used where another map is expected, so long as the target types
    /// are handled in the usual covariant way.
    /// In fact, we require source types to be at least "related" (i.e. in a subtype or
    /// supertype relationship).
    class MapType : public Type {
      public:
        MapType(TypeExp sourceTypeExp, TypeExp targetTypeExp, MapEntryData::Kind defaultFallbackKind = MapEntryData::Kind::All21);

        NewValueHolder createValue(const TypeSystem& typeSystem) const override;

        bool visitValue(const TypeSystem& typeSystem, const Value& v, ChildValueVisitor& visitor) const override;

        std::string getFlavour() const override;

        const TypeExp& getSourceTypeExp() const;
        const TypeExp& getTargetTypeExp() const;

        std::optional<SubtypeOrder> compareSubtypeHelper(const TypeSystem& typeSystem, const Type& other) const override;

        std::string valueToString(const TypeSystem& typeSystem, const ValueHolder& v) const override;
      private:
        TypeExp m_sourceTypeExp;
        TypeExp m_targetTypeExp;
        MapEntryData::Kind m_defaultFallbackKind;
    };
} // namespace babelwires
